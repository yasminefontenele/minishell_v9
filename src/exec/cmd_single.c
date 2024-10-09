/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_single.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 21:03:12 by emencova          #+#    #+#             */
/*   Updated: 2024/10/09 08:49:48 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execute.h"
#include "../../minishell.h"
#include <dirent.h>

static DIR	*check_cmd(t_shell *shell, t_list *comnd, char ***str)
{
	t_exec	*node;
	DIR		*directory;
	char	*path_env;

	directory = NULL;
	node = comnd->content;
	if (node && node->args)
		directory = opendir((const char *)node->args);
	if (node && node->args && ft_strchr(*node->args, '/') && !directory)
	{
		*str = ft_split(*node->args, '/');
		node->path = ft_strdup(*node->args);
		free(node->args[0]);
		node->args[0] = ft_strdup(str[0][form_len(*str) - 1]);
	}
	else if (!built_check(node) && node && node->args && !directory)
	{
		path_env = get_env("PATH", shell->keys, 4);
		*str = ft_split(path_env, ':');
		if (!*str)
			m_error(ERR_PIPE, "Failed to split PATH", 127);
		free(path_env);
		node->path = cmd_find(*str, *node->args, node->path);
	}
	return (directory);
}

int handle_builtins(t_shell *shell, t_list *comnd)
{
    t_exec *node = comnd->content;

    if (built_check(node))
    {
        builtin(shell, comnd, &g_exit_status, ft_strlen(node->args[0]));
        return 1; // Built-in command found and executed
    }
    return 0; // Not a built-in command
}

void remove_quotes_from_args_l(t_exec *node)
{
    int i = 0;
    char *temp;

    while (node->args[i])
    {
        temp = remove_quotes(node->args[i]);
        if (temp)
        {
            free(node->args[i]);
            node->args[i] = temp;
        }
        i++;
    }
}

int check_command(t_shell *shell, t_list *comnd, char ***str)
{
    DIR *directory = check_cmd(shell, comnd, str);

    if (directory)
    {
        closedir(directory);
        return 1; // Command was a directory or not executable
    }
    return 0; // Command is valid
}

void execute_command(t_shell *shell, t_exec *node)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid < 0)
    {
        m_error(ERR_FORK, "Fork failed", 1);
        return;
    }
    else if (pid == 0) // Child process
    {
        execve(node->path, node->args, shell->keys);
        exit(126); // If execve fails, exit with 126
    }
    else // Parent process
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            g_exit_status = WEXITSTATUS(status);
    }
}

void command_get_single(t_shell *shell, t_list *comnd)
{
    t_exec *node;
    char **str = NULL;

    node = comnd->content;
    
    // Check if it's a built-in command
    if (handle_builtins(shell, comnd))
        return;

    // Remove quotes from arguments
    remove_quotes_from_args_l(node);

    // Check if command is valid
    if (check_command(shell, comnd, &str))
        return;

    // Execute the command if valid
    if (node->path && access(node->path, X_OK) == 0)
        execute_command(shell, node);
    else
        m_error(ERR_NEWCMD, node->args[0], 126);

    // Free the dynamically allocated string array
    free_form(&str);
}

/*
static DIR	*check_cmd(t_shell *shell, t_list *comnd, char ***str)
{
	t_exec	*node;
	DIR		*directory;
	char	*path_env;

	directory = NULL;
	node = comnd->content;
	if (node && node->args)
		directory = opendir((const char *)node->args);
	if (node && node->args && ft_strchr(*node->args, '/') && !directory)
	{
		*str = ft_split(*node->args, '/');
		node->path = ft_strdup(*node->args);
		free(node->args[0]);
		node->args[0] = ft_strdup(str[0][form_len(*str) - 1]);
	}
	else if (!built_check(node) && node && node->args && !directory)
	{
		path_env = get_env("PATH", shell->keys, 4);
		*str = ft_split(path_env, ':');
		if (!*str)
			m_error(ERR_PIPE, "Failed to split PATH", 127);
		free(path_env);
		node->path = cmd_find(*str, *node->args, node->path);
	}
	return (directory);
}

static int	handle_builtin_command(t_shell *shell, t_list *comnd, t_exec *node)
{
	if (built_check(node))
	{
		builtin(shell, comnd, &g_exit_status, ft_strlen(node->args[0]));
		return (1);
	}
	return (0);
}

static int	check_and_exec_command(t_shell *shell, t_exec *node)
{
	pid_t	pid;

	if (node->path && access(node->path, X_OK) == 0)
	{
		pid = fork();
		if (pid < 0)
		{
			m_error(ERR_FORK, "Fork failed", 1);
			return (1);
		}
		if (pid == 0)
		{
			execve(node->path, node->args, shell->keys);
			exit(126);
		}
		waitpid(pid, &g_exit_status, 0);
		return (1);
	}
	return (0);
}

static int	check_for_directory(t_shell *shell, t_list *comnd, char ***str)
{
	DIR	*directory;

	directory = check_cmd(shell, comnd, str);
	if (directory)
	{
		closedir(directory);
		return (1);
	}
	return (0);
}

void	command_get_single(t_shell *shell, t_list *comnd)
{
	t_exec	*node;
	char	**str;

	str = NULL;
	node = comnd->content;
	if (handle_builtin_command(shell, comnd, node))
		return ;
	if (check_for_directory(shell, comnd, &str))
		return ;
	remove_quotes_from_args(node);
	if (!check_and_exec_command(shell, node))
		m_error(ERR_NEWCMD, node->args[0], 126);
	free_form(&str);
}


void command_get_single(t_shell *shell, t_list *comnd)
{
    t_exec *node;
    char **str;
    DIR *directory;

    str = NULL;
    node = comnd->content;
    if (handle_builtin_command(shell, comnd, node))
        return;
    directory = check_cmd(shell, comnd, &str);
    if (directory)
    {
        closedir(directory);
        return;
    }
    remove_quotes_from_args(node);
    if (node->path && access(node->path, X_OK) == 0)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            m_error(ERR_FORK, "Fork failed", 1);
            return;
        }
        if (pid == 0)
        {
            execve(node->path, node->args, shell->keys);
            exit(126);
        }
        waitpid(pid, &g_exit_status, 0);
    }
    else
        m_error(ERR_NEWCMD, node->args[0], 126);  
    free_form(&str);
}


void command_get_single(t_shell *shell, t_list *comnd)
{
    t_exec *node;
    DIR *directory;
    char **str;
    pid_t pid;
    int status;

    str = NULL;
    node = comnd->content;
    if (handle_builtin_command(shell, comnd, node))
        return;
    remove_quotes_from_args(node);
    directory = check_cmd(shell, comnd, &str);
    if (directory)
    {
        closedir(directory);
        return;
    }
    if (node->path && access(node->path, X_OK) == 0)
    {
        pid = fork();
        if (pid < 0)
        {
            m_error(ERR_FORK, "Fork failed", 1);
            return;
        }
        else if (pid == 0)
        {
            execve(node->path, node->args, shell->keys);
            exit(126);
        }
        else
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                g_exit_status = WEXITSTATUS(status);
        }
    }
    else
        m_error(ERR_NEWCMD, node->args[0], 126);
    free_form(&str);
}


void command_get_single(t_shell *shell, t_list *comnd)
{
    t_exec *node;
    DIR *directory;
    char **str;
    pid_t pid;
    int status;
    int i;
    char *temp;

    str = NULL;
    node = comnd->content;
    if (built_check(node))
    {
        builtin(shell, comnd, &g_exit_status, ft_strlen(node->args[0]));
        return ;
    }
    i = 0;
    while (node->args[i])
    {
       temp = remove_quotes(node->args[i]);
       if (temp)
        {
            free(node->args[i]);
            node->args[i] = temp;
        }
        i++;
    }
    directory = check_cmd(shell, comnd, &str);
    if (directory)
    {
        closedir(directory);
        return ;
    }
    if (node->path && access(node->path, X_OK) == 0)
    {
        pid = fork();
        if (pid < 0)
        {
            m_error(ERR_FORK, "Fork failed", 1);
            return ;
        }
        else if (pid == 0)
        {
            execve(node->path, node->args, shell->keys);
            exit(126);
        }
        else
        {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                g_exit_status = WEXITSTATUS(status);
        }
    }
    else
        m_error(ERR_NEWCMD, node->args[0], 126);
    free_form(&str);
}*/
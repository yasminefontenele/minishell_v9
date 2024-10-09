/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   p_parser.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yfontene <yfontene@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 18:11:10 by yfontene          #+#    #+#             */
/*   Updated: 2024/10/09 05:37:52 by yfontene         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*parse_simple_token_with_quotes(char *line, int *pos, int keep_quotes)
{
	char	*token;
	char	*quoted_token;

	token = parse_simple_token(line, pos);
	if (line[*pos] == '\'' || line[*pos] == '\"')
	{
		quoted_token = parse_quoted_token(line, pos, keep_quotes);
		token = ft_strjoin(token, quoted_token);
		free(quoted_token);
	}
	return (token);
}

char	*parse_special_token(char *line, int *pos)
{
	if (line[*pos] == '\"' || line[*pos] == '\'')
		return (parse_quoted_token(line, pos, 1));
	else if (line[*pos] == '>' || line[*pos] == '<')
		return (parse_redirect_token(line, pos));
	else
		return (NULL);
}

char	*parse_next_token(char *line, int reset, int keep_quotes)
{
	static int	current_pos;
	char		*token;

	if (reset == 0)
		current_pos = 0;
	current_pos = skip_space(line, current_pos);
	if (!line[current_pos])
		return (NULL);
	token = parse_special_token(line, &current_pos);
	if (token != NULL)
		return (token);
	else
		return (parse_simple_token_with_quotes(line, &current_pos,
				keep_quotes));
}

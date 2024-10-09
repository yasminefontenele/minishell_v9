/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eliskam <eliskam@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 15:48:35 by yasmine           #+#    #+#             */
/*   Updated: 2024/10/09 08:51:34 by eliskam          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include "../exec/execute.h"



char	*extract_substring(char const *s, unsigned int start, size_t end)
{
	char	*sub;
	size_t	i;
	size_t	substring_len;

	substring_len = end - start;
	if (start >= end || substring_len == 0)
		return (NULL);
	sub = malloc(sizeof(char) * (substring_len + 1));
	if (sub == NULL)
		ft_error("malloc error in extract_substring", 1);
	i = 0;
	while (i < substring_len)
	{
		sub[i] = s[start + i];
		i++;
	}
	sub[i] = '\0';
	return (sub);
}

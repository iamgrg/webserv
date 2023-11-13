# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    requestMaker.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gregoire <gregoire@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/28 09:55:00 by gregoire          #+#    #+#              #
#    Updated: 2023/09/28 09:55:01 by gregoire         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

if [ "$#" -ne 1 ] || ! [ "$1" -eq "$1" ] 2> /dev/null; then
  echo "Usage: $0 NUMBER_OF_REQUESTS" >&2
  exit 1
fi

number_of_requests=$1

for ((i=1;i<=number_of_requests;i++))
do
  curl "http://localhost:8080" &
done

# Overview 

Design decisions are written here.

## language and tools

- language: 
    - c++ 
- package manager 
    - vcpkg 
- build tool 
    - cmake 
- ide
    - vscode or visual studio 
- os
    - windows or linux 
- database 
    - mysql 
- cache 
    - redis 

## distributed processing model 

- distributed model
    - authentication server (auth_server)
    - world server (world_server)
        - manages users
        - provide community services 
            - guild, party, chat 
    - game server (game_server)
        - run instances of games 
    - a account database with redis cache
    - per world database

- processing model 
    - asio strand dedicated to each game instance or world service 
    - communication between a world server and game servers 
        - guaranteed failure when link or server down

## logical model 

- service 
    - runs on world and game servers 
    - eg) guild_service, lobby_service, user_service, pc_service, party_service
- system 
    - runs inside a game instance
    - eg) spawn_system, quest_system
- actor 
    - runs inside a game instance 
    - eg) pc, npc, monster, drop_item, spawn_volume, damage_volume
- component 
    - runs inside an actor 
    - eg) inventory_component, skill_component, enchant_item_component

## data model 

excel is supposed as the main tool to edit game data. 
c++ code is generated to manage those exported game data. 

- element 
    - eg) item_element, quest_element
- element_table<>

- column types 
    - numbers
    - string
    - enum 
    - array 
    - map 
- column constraints
    - primary key
    - unique key 
    - key 
    - foreign key


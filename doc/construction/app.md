# app 

- play::robust::app 

- server 
    - topic_handler 

- object
    - track objects

- app::logger 
    - filtering with category
    - ... more use cases required

- service 
    - db_service
    - auth_service
    
    - world_user_service 
    - world_rpc_service
    - world_instance_service 
        - instance_state

    - game_entrance_service
    - game_lobby_service
    - game_instance_service 
        - instance

- system 
    - quest_system 
    - event_system 
    - drop_system 
    - spawn_system 

- actor 
    - world_user
    - game_user
    - instance
        - character
            - pc
            - npc
            - monster
        - spawn_volume 
        - damage_volume
        
controller controls actor. 

- components 
    - inventory_controller
    - ability_controller 
    - skill_controller 
    - buff_debuff_controller
    - enchant_item_controller
    - craft_item_controller
    - quest_controller
    - behavior_controller
    - ... 

- utility 
    - various utility classes
    

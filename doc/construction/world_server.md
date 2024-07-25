# world server 

## services 

- world_user_service
    - world_user (actor)
        - pc_list_controller 
            - list / load 
            - create / delete 
            - select
- world_instance_service
- guild_service
- party_service 
- chat_service
 
## how continuations work? 

- pc_list_controller 
    - has [pc_base_property]
    - create 
        - db_service::post(exec_func, callback_func)
        - call back on_db_created(pc_base_property, result)
            - aded to the list
            - send response
                - world_user::send(res_pc_create)
    
- pc_list_controller 
    - select 
        - request select to db_service    
        - call back 

- world_user 
    - start game 
        - world_instance_service::post( select_instance, callback )
            - game_server_service::get_server(id).send()
        - hanler to world_instance_service::on_created(result, info)
            - use rpc id to find the request 
            - calls back the function on the world_user
                - mark worl_user as joining to the instance
                - client joins the instance 
                    - instance sync with world_instance_service 

- game_user 
    - leave game
        - instance 

The detailed implementation is also important. 
play implementation with lambda is the core. 
- capture actors to safely callback 
- handle edge cases 



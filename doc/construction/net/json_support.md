# json support 

plain_protocol에서 topic 타잎이 integral_type이 아닐 경우 
topic 값을 0으로 하여 전달하고 받고 topic 전달을 topic{} 기본값으로 
한다. 

session_handler에서 on_receive()로 받은 값에 대해 문자열 처리하고 
토픽 필드를 알고 전달할 수 있는 frame_handler를 만들어 
session_handler에 알려서 처리한다. 

현재 session_handler인 client, server 각각에 포함하면 처리 가능하다. 



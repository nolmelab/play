# json support 

plain_protocol에서 topic 타잎이 integral_type이 아닐 경우 
topic 값을 0으로 하여 전달하고 받고 topic 전달을 topic{} 기본값으로 
합니다. 

session_handler에서 on_receive()로 받은 값에 대해 문자열 처리하고 
토픽 필드를 알고 전달할 수 있는 frame_handler를 만들어 
session_handler에 알려서 처리합니다.

현재 session_handler인 client, server 각각에 포함하면 처리 가능합니다. 

sodium_protocol도 동일하게 처리 가능하므로 같은 기능을 포함하도록 합니다. 

거의 필요한 대부분의 기능을 갖추게 됩니다. 

테스트를 완전하게 합니다. 테스트를 통한 개선이 가장 중요합니다. 
코딩을 잘 못 하고 많이 부족한 나는 특히 더 필요합니다. 



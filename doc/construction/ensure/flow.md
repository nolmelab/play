# flow 

flow는 테스트 흐름이라는 뜻으로 HFSM을 json으로 정의하여 실행하는 구조입니다. 

flow는 실행할 일련의 act들을 갖습니다. bot에 생성하고 참조합니다. 

act들은 act_factory에 등록하고 act_factory를 통해 생성합니다. 
생성은 함수를 통해 합니다. bot::on_created_act()에서 적절한 
이름으로 보관하고 flow에 지정한 이름으로 찾을 수 있도록 인덱싱합니다. 

flow가 HFSM으로 동작하는 방법은 시그널과 타겟 액트를 찾아 activate 하는 
구조입니다. act_ensure::signal() 함수에서 bot을 통해 flow에 전달하면 
정의된 전환 정보에 따라 현재 act를 deactivate하고 타겟 act를 찾아 activate합니다. 

act_ensure::activate()는 기본 반복 타이머를 등록합니다. 
이 타이머를 통해 timeout 처리 등을 진행합니다. 


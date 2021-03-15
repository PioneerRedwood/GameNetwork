# Game Network
### 2021.03.07 ~ 2021.03.15

유니티와 연동 가능한 게임 네트워크 모듈, 그와 관련된 서버와 클라이언트 제작



##### 2021.03.08

[Character Sets](https://stackoverflow.com/questions/700187/unicode-utf-ascii-ansi-format-differences) 포맷을 어떤 것으로 지정하느냐에 따라 Low level에서의 네트워크 함수 사용이 달라진다. 가령 GetAddrInfoW(Unicode), getaddrinfo(ANSI)와 같이 말이다. 이 둘의 매개변수의 수와 하는 역할은 같지만 매개변수의 타입이 달라진다. 



TODO: @최적화

- 서버에 접속된 클라이언트에 대한 정보를 담을 컨테이너가 필요
  - 대체로 검색/삽입/삭제가 O(log n)정도의 속도를 평균적으로 유지하며 정렬할 필요가 없는 [unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set)이 적합해 보인다.
- 이를 Server내 Loop에서 key 값을 순회하여 fd의 변화를 읽도록 한다.



##### 2021.03.09

유니티 내에서 C++ 코드를 사용하기 위해선 dll로 만들어서 plugin 시키는 방법이 있다. 

빌드를 Release x64와 dll(exe이 아닌)로 맞춘 후 프로젝트의 전처리기를 설정 및 선언해주거나 유니티에서 호출하는 함수 앞에 __declspec(dllexport)의 전처리기를 붙여야한다.

현재 유니티를 윈도우로 빌드시켜 2개의 채팅 클라이언트를 만들고 콘솔 창으로 채팅 서버를 작동시키는 방안을 모색중이다.

지금 막힌 부분은 오가는 패킷의 양이 어마무시할때 한두번 받아선 통신할 수 없을 것이다.



##### 2021.03.13

네트워크 데이터 패킹에는 왕도가 있다? 없다? http://www.sdltutorials.com/cpp-tip-packing-data

플랫폼을 가리지 않는 소켓 프로그래밍 https://web.archive.org/web/20180823021126/https://gafferongames.com/post/sending_and_receiving_packets/ 해당 웹사이트는 도움되는 글들이 많다.



##### 2021.03.14

클라이언트는 유니티 C#으로 통신을 할 예정, [Google의 protobuf](https://developers.google.com/protocol-buffers/docs/cpptutorial#where-to-find-the-example-code)를 진행하고 있다. 



##### 2021.03.15

유니티 클라이언트 제작해서 C++ 서버와 간단한 시간 데이터를 송수신하는 것을 구현했다. 이제 더는 해당 레포지토리에 푸시는 없다. [학교 내 진행하는 프로젝트](https://github.com/PioneerRedwood/Bletchley에 집중한다.


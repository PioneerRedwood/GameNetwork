# Game Network
### 2021.03.07 ~

유니티와 연동 가능한 게임 네트워크 모듈, 그와 관련된 서버와 클라이언트 제작



##### 2021.03.08

[Character Sets](https://stackoverflow.com/questions/700187/unicode-utf-ascii-ansi-format-differences) 포맷을 어떤 것으로 지정하느냐에 따라 Low level에서의 네트워크 함수 사용이 달라진다. 가령 GetAddrInfoW(Unicode), getaddrinfo(ANSI)와 같이 말이다. 이 둘의 매개변수의 수와 하는 역할은 같지만 매개변수의 타입이 달라진다. 



TODO: @최적화

- 서버에 접속된 클라이언트에 대한 정보를 담을 컨테이너가 필요
  - 대체로 검색/삽입/삭제가 O(log n)정도의 속도를 평균적으로 유지하며 정렬할 필요가 없는 [unordered_set](https://en.cppreference.com/w/cpp/container/unordered_set)이 적합해 보인다.
- 이를 Server내 Loop에서 key 값을 순회하여 fd의 변화를 읽도록 한다.
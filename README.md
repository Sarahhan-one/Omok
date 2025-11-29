# Omok (오목) - Network Multiplayer Game
<img width="1000" height="200" alt="Image" src="https://github.com/user-attachments/assets/a12574f7-880a-4737-8830-1668c81bb765" />
<img width="1000" height="200" alt="Image" src="https://github.com/user-attachments/assets/afb81215-ed76-4342-b7b2-d6368b838326" />
<img width="1000" height="200" alt="Image" src="https://github.com/user-attachments/assets/cdad1583-6263-43e4-9054-dfba8c816f0e" />

C++과 SFML 네트워크 라이브러리를 활용하여 제작한 **실시간 멀티플레이어 오목 게임**입니다.  
TCP 소켓 프로그래밍을 통한 네트워크 통신과 Windows Console API를 활용한 콘솔 UI를 구현했습니다.

---

## **프로젝트 개요**

친구들과 함께 플레이할 수 있는 네트워크 기반 오목 게임을 제작했습니다.   
서버-클라이언트 아키텍처를 사용하여 실시간 대전이 가능하며, 게임 종료 후 재대결 기능까지 구현했습니다.

```
┌─────────────┐          TCP Socket          ┌─────────────┐
│   Server    │ ←─────────────────────────→ │   Client    │
│  (흑돌 선공)  │     Port: 53000             │  (백돌 후공)  │
│             │   Packet: row, col          │             │
│ isMyTurn=T  │                             │ isMyTurn=F  │
└─────────────┘                             └─────────────┘
```

---

## **주요 기능**

- **TCP 소켓 기반 실시간 멀티플레이어**: 서버-클라이언트 구조로 네트워크 대전 지원
- **턴제 게임 로직**: 비동기 입력 처리와 턴 동기화 구현
- **5목 승리 판정**: 4방향 탐색 알고리즘을 통한 O(1) 시간복잡도 승리 조건 검증
- **재대결 시스템**: 양측 동의 기반 게임 리셋 및 재시작 프로토콜
- **실시간 콘솔 UI**: Windows Console API를 활용한 커서 제어 및 색상 표시
- **에러 핸들링**: 연결 실패, 패킷 손실 등 네트워크 예외 상황 처리

---

## **개발 환경**

- **언어**: C++17
- **라이브러리**: SFML 2.5.1 (Network 모듈)
- **IDE**: Visual Studio 2022

---

## **기술적 구현**

이 프로젝트는 **네트워크 동기화·실시간 입출력·알고리즘 최적화**를 고려해 설계되었습니다.

### 1. MVC 패턴 기반 아키텍처 설계

프로젝트는 **Separation of Concerns** 원칙에 따라 3개의 클래스로 구조화했습니다:

- **GameManager (Controller)**: 게임 로직, 네트워크 통신, 턴 관리를 총괄
  - `Role` enum으로 서버/클라이언트 역할 구분
  - `isMyTurn` 플래그를 통한 턴 기반 상태 관리
  - 게임 루프의 핵심 흐름 제어

- **Board (Model)**: 15×15 게임 보드 상태 관리
  - `vector<vector<char>>` 자료구조로 2차원 보드 표현
  - 경계 검사와 중복 배치 검증 로직 캡슐화
  - 불변성 보장을 위한 const 메서드 설계

- **Display (View)**: Windows Console API 기반 렌더링
  - `SetConsoleCursorPosition()`으로 특정 좌표에 직접 출력
  - 색상 변경으로 흑돌/백돌 구분
  - 부분 렌더링 최적화로 깜빡임 최소화

**설계 이점**:
- 각 클래스의 책임이 명확하여 유지보수성 향상
- 네트워크 로직과 UI 로직의 독립성 확보
- 단위 테스트 및 기능 확장 용이

---

### 2. TCP 소켓 프로그래밍

SFML Network 모듈을 활용한 블로킹 소켓 방식으로 연결이 확정된 후 게임을 시작합니다.  

#### **서버-클라이언트 연결 수립**

- **서버 모드**:
  - `TcpListener`로 지정 포트에서 연결 요청 대기
  - 클라이언트 연결 시 `TcpSocket` 객체 생성
  - 연결 성공 시 상대방 IP 주소 출력

- **클라이언트 모드**:
  - 사용자가 입력한 IP 주소로 서버 접속
  - 연결 실패 시 에러 메시지와 함께 종료

---

### 3. 패킷 기반 데이터 직렬화

SFML의 `Packet` 클래스를 사용하여 구조화된 데이터 전송을 구현했습니다.

#### **게임 수 전송**

```cpp
Packet packet;
packet << currentRow << currentCol;  // 좌표 직렬화
socket.send(packet);                 // 전송
```

- `operator<<`를 통한 자동 직렬화 (Endianness 자동 처리)
- 정수 2개(행, 열)를 하나의 패킷으로 묶어 전송
- 원자적(atomic) 전송 보장

#### **상대방 수 수신**

```cpp
Packet packet;
socket.receive(packet);              // 블로킹 수신
int row, col;
packet >> row >> col;                // 역직렬화
```

- 데이터가 도착할 때까지 대기 
- `operator>>`로 순서대로 데이터 추출

---

### 4. 턴 기반 동기화 메커니즘

두 플레이어 간의 턴을 동기화하기 위해 **State Machine** 패턴을 적용했습니다.

#### **턴 관리 흐름**

```
[내 턴] → 돌 배치 → 좌표 전송 → isMyTurn = false
                                    ↓
[상대 턴] ← 좌표 수신 ← 대기 ← isMyTurn = false
     ↓
승리 판정 → 턴 전환 → isMyTurn = true
```

- **내 턴 처리**:
  ```cpp
  if (isMyTurn) {
      handleInput();           // 비블로킹 키 입력
      placeStone();            // 돌 배치 + 전송
      isMyTurn = false;        // 턴 종료
  }
  ```

- **상대 턴 처리**:
  ```cpp
  else {
      waitForOpponent();       // 블로킹 수신
      isMyTurn = true;         // 턴 획득
  }
  ```

**동기화 전략**:
- 서버는 `isMyTurn = true`로 시작 (선공 보장)
- 클라이언트는 `isMyTurn = false`로 시작 (후공 보장)
- 돌 배치 후 즉시 턴을 넘겨 **데드락 방지**
- 그래서 네트워크 지연이 있어도 턴 순서는 절대 뒤바뀌지 않음

---

### 5. 승리 판정

오목의 승리 조건(5개 연속)을 효율적으로 검증하기 위해 **방향 벡터 기반 탐색**을 구현했습니다.  전체 보드 탐색하지 않고, 마지막에 놓은 돌 위치에서 4방향 × 최대 8칸 = 32회 비교 검사합니다. 

---

### 6. 재대결 동기화

게임 종료 후 양측의 재대결 의사를 동기화하기 위한 **양방향 핸드셰이크 프로토콜**을 설계했습니다.  양측 동의 확인 후 재시작합니다.  

- 전송 → 수신 순서를 양측에서 동일하게 유지해서 양측이 모두 수신 대기하면서 발생 가능한 교착 상태를 방지  
- 수신 실패 시엔 재대결 거부로 간주하고 상대방이 연결을 끊었을 때 무한 대기 방지

---

### 7. 입력 처리

사용자 입력과 네트워크 통신을 **Polling 방식**으로 처리했습니다.

#### **게임 루프 구조**

```cpp
while (!isBattleEnded) {
    if (isMyTurn) {
        // 키 입력 확인 (비블로킹)
        if (currentRow != prevRow || currentCol != prevCol) {
            updateScreen();  // 변경된 경우에만 렌더링
        }
        handleInput();
    } else {
        waitForOpponent();  // 블로킹 수신
        updateScreen();
    }
    Sleep(100);  // 100ms 폴링 주기
}
```

---

### 8. 렌더링

콘솔 환경에서 GUI 같은 사용자 경험을 제공하기 위해 Windows API를 활용해 실시간 화면 업데이트를 했습니다. 커서 위치가 변경된 경우에만 화면을 업데이트하고, 전체 화면을 다시 그리는 대신 변경된 셀만 업데이트함으로 화면 깜박임을 방지했습니다. 

---

## **설치 및 실행**

### 설치 방법

1. **리포지토리 클론**
   ```bash
   git clone https://github.com/Sarahhan-one/Omok.git
   cd Omok
   ```

2. **SFML 설정**
   - [SFML 다운로드](https://www.sfml-dev.org/download.php) (Visual C++ 버전)
   - 프로젝트 속성 설정:
     - `C/C++ → 추가 포함 디렉터리`: `SFML/include`
     - `링커 → 추가 라이브러리 디렉터리`: `SFML/lib`
     - `링커 → 추가 종속성`: `sfml-network.lib` 추가

3. **빌드 및 실행**
   - `Omok.sln` 열기
   - 디버그나 실행

### 게임 시작

**서버**:
```
프로그램 실행 → 'S' 입력 → 포트 53000에서 대기
```

**클라이언트**:
```
프로그램 실행 → 'C' 입력 → 서버 IP 입력 (로컬: 127.0.0.1)
```

---

## **게임 방법**

### 조작법
- **방향키 (↑↓←→)**: 커서 이동
- **스페이스바**: 돌 배치
- **재대결**: 게임 종료 후 아무 키나 입력

### 규칙
- 서버: 흑돌(B) / 클라이언트: 백돌(W)
- 흑돌 선공
- 가로/세로/대각선 5개 연속 시 승리
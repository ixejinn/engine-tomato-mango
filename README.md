# 🍅 Tomato Engine

**C++ 기반 개인 게임 엔진 개발 프로젝트**  
*A personal game engine project built with C++ and OpenGL.*

[![C++](https://img.shields.io/badge/C++-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?style=flat-square&logo=opengl&logoColor=white)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org/)

---

## 📌 소개 | About

C++와 OpenGL을 기반으로 개발 중인 개인 게임 엔진 프로젝트입니다.  
엔진의 구조와 핵심 시스템을 직접 설계하고 구현하며, 재사용 가능한 게임 엔진을 만드는 것을 목표로 합니다.

*A personal game engine built with C++ and OpenGL, focusing on engine architecture, core systems, and reusability.*

## 🌱 개발 목표 | Goals

- 모듈화되고 재사용 가능한 엔진 아키텍처
- ECS 기반 시스템 설계
  
## 🛠️ 주요 기능 | Features

- **Core Engine | 엔진 코어**
  - ECS 기반 엔티티 및 컴포넌트 관리
  - 시스템 기반 게임 루프 및 업데이트 파이프라인

- **Input System | 입력 시스템**
  - 키보드 및 마우스 입력 처리
  - 엔진 업데이트 루프와 연동된 입력 관리
    
- **Rendering | 렌더링**
  - OpenGL 기반 그래픽 렌더링
  - 텍스처, 셰이더 및 폰트 관리
    
- **Physics & Collision | 물리 및 충돌**
  - 오브젝트 Transform 관리
  - 충돌 감지 및 처리
    
- **UI System | UI 시스템**
  - 계층 구조 기반 UI 관리
  - UI 레이아웃, 렌더링 및 이벤트 처리
  
- **Scene & Resource Management | 씬 및 리소스 관리**
  - 씬 저장 및 불러오기
  - JSON 기반 씬 직렬화

- **Editor | 에디터**
  - ImGui 기반 에디터 도구
  - 엔티티 계층 구조 및 컴포넌트 편집

---

## 💻 기술 스택 | Tech Stack

| Category | Technologies |
|:--|:--|
| Language | C++ |
| Graphics | OpenGL |
| ECS | EnTT |
| Editor UI | Dear ImGui |
| Text Rendering | FreeType |
| Serialization | nlohmann/json |
| Build System | CMake |

---

<p align="center">
  <sub>🚧 Currently under development</sub>
  <br/>
  <sub>지속적으로 개발 및 개선 중입니다.</sub>
</p>

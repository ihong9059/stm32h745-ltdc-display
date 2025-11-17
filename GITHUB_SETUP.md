# GitHub 저장소 설정 가이드

## 📝 GitHub 저장소 생성 방법

### 1. GitHub 웹사이트에서 저장소 생성

1. https://github.com 접속 후 로그인 (ihong9059 계정)
2. 우측 상단 '+' 버튼 클릭 → "New repository" 선택
3. 다음과 같이 설정:
   - **Repository name:** `stm32h745-ltdc-display`
   - **Description:** "STM32H745I-DISCO LTDC Display Project with RGB565 image on RK043FN48H LCD"
   - **Public** 또는 **Private** 선택
   - **❌ Initialize this repository with:** 모두 체크 해제 (README, .gitignore, license)
   - "Create repository" 클릭

### 2. 로컬 저장소와 연결

저장소 생성 후 터미널에서 다음 명령 실행:

```bash
cd /Users/maeg/todo/stm32/ltdc

# GitHub 저장소와 연결
git remote add origin https://github.com/ihong9059/stm32h745-ltdc-display.git

# 메인 브랜치로 변경
git branch -M main

# GitHub에 푸시
git push -u origin main
```

### 3. 인증 방법

#### Option A: Personal Access Token (추천)
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. "Generate new token (classic)" 클릭
3. Note: "STM32 LTDC Project"
4. Expiration: 90 days (또는 원하는 기간)
5. Scopes: **repo** 체크
6. "Generate token" 클릭
7. 토큰 복사 (한 번만 표시됨!)
8. Push 시 비밀번호 대신 토큰 입력

#### Option B: SSH Key
```bash
# SSH 키 생성 (없는 경우)
ssh-keygen -t ed25519 -C "ihong9059@github.com"

# 공개 키 확인
cat ~/.ssh/id_ed25519.pub

# GitHub에 SSH 키 등록
# Settings → SSH and GPG keys → New SSH key
# 복사한 공개 키 붙여넣기

# Remote URL을 SSH로 변경
git remote set-url origin git@github.com:ihong9059/stm32h745-ltdc-display.git
```

---

## 🚀 현재 상태

✅ 로컬 Git 저장소 초기화 완료
✅ 파일 커밋 완료
❌ GitHub 저장소 생성 대기 중
❌ Push 대기 중

---

## 📦 커밋된 파일 목록

```
23 files committed:
- .gitignore
- README.md
- SESSION_REPORT.md
- NEXT_SESSION_GUIDE.md
- LTDC_Layer_Guide.html
- CM7/Core/Src/*.c (9 files)
- CM7/Core/Inc/*.h (8 files)
- CM7/STM32H745XIHX_FLASH.ld
```

---

## 🔄 Push 후 확인사항

Push 완료 후 다음을 확인하세요:

1. GitHub 저장소 페이지에서 파일 확인
2. README.md가 자동으로 표시되는지 확인
3. 코드 브라우징 가능 확인

---

## 📚 추가 Git 명령어

### 상태 확인
```bash
git status
git log --oneline
git remote -v
```

### 변경사항 추가 커밋
```bash
git add .
git commit -m "Update: description"
git push
```

### 브랜치 관리
```bash
git branch                    # 브랜치 목록
git checkout -b feature-name  # 새 브랜치 생성 및 이동
git merge feature-name        # 브랜치 병합
```

---

## ⚠️ 주의사항

1. **민감한 정보 제외**
   - 이미 .gitignore에 빌드 파일 제외됨
   - API 키, 비밀번호 등 포함 금지

2. **대용량 파일**
   - RGB565_480x272.h (766KB) 포함됨
   - GitHub는 100MB 이상 파일 거부

3. **커밋 메시지**
   - 명확하고 간결하게 작성
   - 한글 또는 영문 사용 가능

---

**작성일:** 2025년 11월 17일
**저장소:** https://github.com/ihong9059/stm32h745-ltdc-display (생성 대기 중)

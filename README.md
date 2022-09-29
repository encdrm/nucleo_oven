# Nucleo-Oven

## Naming convention

- Object initializer 이름은 "Custom_" prefix가 붙는 PascalCase
- Object 이름과 typedef는 소문자
- 메서드는 전부 소문자, Object 이름이 prefix로 붙는 snake_case
  - ex) [Object 이름]_[메서드 이름]
- 함수는 PascalCase
- 변수는 camelCase
- Define은 전부 대문자

## To-do

- STM32CubeMX에서의 별도의 설정 없이도 heater와 tempsensor가 정상적으로 initialize될 수 있도록 하기 -> 완전한 라이브러리화

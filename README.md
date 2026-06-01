# Домашние задания по дисциплине «Алгоритмы и структуры данных»

## Состояние

- [![BuildUtestItest](../../actions/workflows/make_and_test.yml/badge.svg)](../../actions/workflows/make_and_test.yml) — Make и тесты
- [![StyleCheck](../../actions/workflows/lint_style_check.yml/badge.svg)](../../actions/workflows/lint_style_check.yml) — CLang Tidy и форматирование

## Задания

### [02.UTF-encoder](02.UTF-encoder) СЛУЧАЙНО СДЕЛАНО ВМЕСТО Base85

Кодирование и декодирование между UTF-8, UTF-16LE и UTF-32LE.

1. Библиотека `utf_codec` (namespace `utf`).
2. CLI `utfencoder` с режимами `-8to16`, `-16to8`, `-8to32`, `-32to8`, `-16to32`, `-32to16` (и длинными именами `--utf8-to-utf16` и т.д.).
3. Юниттесты (Google Test) и интеграционный тест с Python-oracle.

### [03.Gauss](03.Gauss)

Решение системы методом Гаусса

- Условие читаем из файла `.csv`
- Пользуясь возможностями Eigen, оперируем строками (имеем в виду, что при должных настройках нам доступна аппаратная векторизация)

При этом:

- Снабжаем решение модульными тестами
- Большие системы генерируем при помощи [генератора воспроизводимых псевдослучайных чисел](https://stackoverflow.com/a/37945656)
- Результат также выводим в формате CSV
- Интеграционные проверяем при помощи `cmp`, как с BASE85

## Что рекомендуется установить и как пользоваться?

### Что установить

1. Разумеется, Git.
2. Linux. С ним легче пойдёт.
3. То, что [в зависимостях](.github/actions/install-prerequisites/action.yml).
4. Visual Studio Code
   - [плагин CodeLLDB](https://open-vsx.org/extension/vadimcn/vscode-lldb)
   - [плагин Astyle](https://open-vsx.org/extension/haloscript/astyle-lsp-vscode)

### Как пользоваться

Просто. Для каждого каталога не забывайте запускать:

- `make all`
- `make astyle`
- `make tidy`

и внимательно смотреть, что произошло.

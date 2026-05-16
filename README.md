# Домашние задания по дисциплине «Алгоритмы и структуры данных»

## Состояние

- ![BuildUtestItest](https://github.com/dmsavkov/Encoder-Decoder-UTF/actions/workflows/make_and_test.yml/badge.svg) — Make и тесты
- ![StyleCheck](https://github.com/dmsavkov/Encoder-Decoder-UTF/actions/workflows/lint_style_check.yml/badge.svg) — CLang Tidy и форматирование

> **GitHub Actions на форке:** если в [Actions](https://github.com/dmsavkov/Encoder-Decoder-UTF/actions) пусто, откройте **Settings → Actions → General** и включите *Allow all actions and reusable workflows*, затем **Run workflow** (кнопка появится после `workflow_dispatch` в YAML) или сделайте push в `main`.
>
> **PR в `dluciv/cpp-tasks-seed`:** проверки могут ждать одобрения преподавателя (*action required*) — это нормально для PR из форка.

## Задания

### [02.UTF-encoder](02.UTF-encoder) СЛУЧАЙНО СДЕЛАНО ВМЕСТО Base85

Кодирование и декодирование между UTF-8, UTF-16LE и UTF-32LE.

1. Библиотека `utf_codec` (namespace `utf`).
2. CLI `utfencoder` с режимами `-8to16`, `-16to8`, `-8to32`, `-32to8`, `-16to32`, `-32to16` (и длинными именами `--utf8-to-utf16` и т.д.).
3. Юниттесты (Google Test) и интеграционный тест с Python-oracle.

### ... Добавляйте следующие аналогично...

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

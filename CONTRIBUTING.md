# Contributing

When contributing to this repository, please first discuss the change you wish to make with an issue.

Please note we have a code of conduct, please follow it in all your interactions with the project.

# Pull Request Process

- If a pull request is to fix a current issue, that issue shall be linked to the pull request.

- The pull request must be approved by the main developer or atleast two other developers of this project

- Upon approval merges will be done by fast-forward.

# Code of Conduct

When in question follow the current existing style if not 
explicitly stated. 

The following case types must be used:

- Macros = UPPER_CASE_SNAKE_CASE
- Global Functions = PascalCase
- Local Functions = camelCase
- Local Variables = camelCase

Comments describing data structures, functions and specific algorithms must be in block comment form.

Long strings of conditional operators split into multiple lines must be in comparison to the following:

```
if (foo->next == NULL
    && totalcount < needed && needed <= MAX_ALLOT
    && server_active(current_input))
   {
    ...
```

Logical operators are located on the following line of a line break.

Tabstops must be 4 white spaces.
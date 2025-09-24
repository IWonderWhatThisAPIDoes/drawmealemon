# Draw Me a Lemon

Visualization of the execution of parsers made with the Lemon parser generator.
A handy tool for debugging grammars, or just exploring how the parser generator works.

```
          INPUT || STACK
                ||--,
                || 0|
-> Begin        ||  |
    `-----------||  |--,
                ||  | 2|
-> One          ||  |  |
   |        ,-* ||  |  | lines ::=
   |      lines ||  |  |
   |        `---||  |  |--,
   |            ||  |  | 1|
    `-----------||  |  |  |--,
                ||  |  |  | 4|
-> Two          ||  |  |  |  |
    `-----------||  |  |  |  |--,
                ||  |  |  |  | 3|
-> Two          ||  |  |  |  |  |
   |        ,-* ||  |  |  |xx+xx+ Syntax error
   |      error ||  |  |  |
   |        `---||  |  |  |--,
   |            ||  |  |  | R|
   |        ,---||  |  |--+--` lines ::= lines line
   |      lines ||  |  |
   |        `---||  |  |--,
   |            ||  |  | 1|
   X            ||  |  |  | Syntax error
                ||  |  |  |
-> End          ||  |  |  |
    `-----------||  |  |  |--,
                ||  |  |  | R|
            ,---||  |--+--+--` block ::= Begin lines End
          block ||  |
            `---||  |--,
                ||  | 5|
-> $            ||  |  |
   |        ,---||  |--` start ::= block
   |      start ||  |
   |        `---||  |--,
   |            ||  | R|
   X            ||--+--`
                || Accept!
```

## Usage

```sh
./drawmealemon <grammarFile> [options] -- [tokensToParser]
```

`grammarFile` - Lemon grammar file that describes the parser

`tokensToParser` - Tokens sent to the parser in the sample session that will be visualized

### Options
| Flag           | Description           |
|----------------|-----------------------|
| `-h, --help`   | Print the usage and exit |
| `-t, --target` | Specify the output format (see below) |
| `-o, --option` | Options that further customize the output format (see below) |

### Output Formats

`-t ascii` - Outputs an ASCII art of the parser's execution.

| Option      | Description |
|-------------|-------------|
| `-o iw=<n>` | Sets the width (in characters) of the left (input) column |

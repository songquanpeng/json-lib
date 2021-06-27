# JSON Lib

## JSON Grammar
```
JSON-text = ws value ws
ws = *(%x20 / %x09 / %x0A / %x0D)
value = null / false / true / number
null  = "null"
false = "false"
true  = "true"
number = [ "-" ] int [ frac ] [ exp ]
int = "0" / digit1-9 *digit
frac = "." 1*digit
exp = ("e" / "E") ["-" / "+"] 1*digit

```

## Reference
https://github.com/miloyip/json-tutorial
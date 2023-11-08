package main

import (
    "fmt"
    "demo/calculator" // Replace with the actual import path
)

func main() {
    x, y := 15, 3

    fmt.Println("Addition:", calculator.Add(x, y))
    fmt.Println("Subtraction:", calculator.Subtract(x, y))
    fmt.Println("Multiplication:", calculator.Multiply(x, y))
    fmt.Println("Division:", calculator.Divide(x, y))
}

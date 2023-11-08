package main

import (
    "fmt"
    "time"
)

func main() {
    go sayHello()

    // Wait for a second to give the goroutine time to execute
    time.Sleep(1 * time.Second)
    fmt.Println("Main function")
}

func sayHello() {
    fmt.Println("Hello from goroutine")
}

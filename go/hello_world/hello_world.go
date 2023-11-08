package main

import(
	"fmt"
	"runtime"
)

func printFunctionName() {

	// Get the program counter
	pc, _, _, _ := runtime.Caller(1)
	// Retrieve *runtime.Func object associated with the caller.
	funcObj := runtime.FuncForPC(pc)

	// Get the name of the function
	funcName := funcObj.Name()

	fmt.Println("Function Name:", funcName)
}

func main() {

	printFunctionName()

	fmt.Println("Hello, World!")
}

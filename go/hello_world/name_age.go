package main

import (
	"encoding/json"
	"fmt"
	"log"
)

type Person struct {
	Name string `json:"name"`
	Age  int    `json:"age"`
}

func main() {

	// JSON data
	jsonData := []byte(`{"name": "John Doe", "age": 30}`)

	// Variable to hold decoded data
	var person Person

	// Unmarshal the JSON into the person variable
	err := json.Unmarshal(jsonData, &person)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("Name: %s, Age: %d\n", person.Name, person.Age)
}

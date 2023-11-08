package main

import (
    "fmt"
    "io/ioutil"
    "net/http"
    "crypto/tls"
    "os/exec"
    "strings"
    "bytes"
)

// Run command and return stdout, stderr and error
func Do(cmd string) (string, string, error) {

    args := strings.Fields(cmd)
    runCMD := exec.Command(args[0], args[1:]...)
    var stdout, stderr bytes.Buffer
    runCMD.Stdout = &stdout
    runCMD.Stderr = &stderr
    err := runCMD.Run()
    outStr, errStr := stdout.String(), stderr.String()
    return outStr, errStr, err

}

func GetBootstrapClusToken() (string, error) {

    cmd := "kubectl create token cmo-bootstrap-admin --namespace cmo-bootstrap --duration=48h"
    stdout, _, err := Do(cmd)
    if err !=nil {
        return "", err
    }

    return strings.TrimSpace(stdout), nil
}

func main() {
    // Replace these variables with your actual data
    kubernetesAPIServer := "https://10.88.0.2:6443" // Kubernetes API server URL
    namespace := "gc"                               // Namespace to list pods from

    // Service account token
    //token := "eyJhbGciOiJSUzI1NiIsImtpZCI6Ik9HU2gzRXQtdXd6M1hQZ2paS0ItdWY2eU9uNVhqOFZzWFZSU0Y2RmNfdEEifQ.eyJhdWQiOlsiaHR0cHM6Ly9rdWJlcm5ldGVzLmRlZmF1bHQuc3ZjLmNsdXN0ZXIubG9jYWwiXSwiZXhwIjoxNjk5ODg2OTQ2LCJpYXQiOjE2OTk4NDM3NDYsImlzcyI6Imh0dHBzOi8va3ViZXJuZXRlcy5kZWZhdWx0LnN2Yy5jbHVzdGVyLmxvY2FsIiwia3ViZXJuZXRlcy5pbyI6eyJuYW1lc3BhY2UiOiJjbW8tYm9vdHN0cmFwIiwic2VydmljZWFjY291bnQiOnsibmFtZSI6ImNtby1ib290c3RyYXAtYWRtaW4iLCJ1aWQiOiJlY2Q1NGUzYS04M2I2LTRmNmMtYjAyNi1iZmUzZTBlODAzZTEifX0sIm5iZiI6MTY5OTg0Mzc0Niwic3ViIjoic3lzdGVtOnNlcnZpY2VhY2NvdW50OmNtby1ib290c3RyYXA6Y21vLWJvb3RzdHJhcC1hZG1pbiJ9.PYAXCIWScna6GF7vru3ID9hnm0iH1AVGcPfAu0U-6tfuQ7GbK-uO90VAGaEF75EvnAvXXQsQzxP6heR9-ykoP-8heRB6tND6aU4aYS73kzFvZDMfF6mtyPbrGmKZGWj2ts2D1QiknRJ_zqpjAEq0omR4QsfGoL71Iy3uQACb7OEm0FXIESZn6rjNNvkhUd2Tc6mYsc1JSttyIV1pIB1i-3JkmHy4QhfjNsFBIyIWCLgvURW2Dw4TQjWW8X_GkoQTjSslCxRu4K6oATjuEB8rA4PUxyciI4-kAIUEoSC7_B2AeqwVKNH5lukD0S6fN85I4Fx_iMl8p7n4wLEiBIaJJw"

    //token := "eyJhbGciOiJSUzI1NiIsImtpZCI6Ik9HU2gzRXQtdXd6M1hQZ2paS0ItdWY2eU9uNVhqOFZzWFZSU0Y2RmNfdEEifQ.eyJhdWQiOlsiaHR0cHM6Ly9rdWJlcm5ldGVzLmRlZmF1bHQuc3ZjLmNsdXN0ZXIubG9jYWwiXSwiZXhwIjoxNjk5ODM2OTc4LCJpYXQiOjE2OTk3OTM3NzgsImlzcyI6Imh0dHBzOi8va3ViZXJuZXRlcy5kZWZhdWx0LnN2Yy5jbHVzdGVyLmxvY2FsIiwia3ViZXJuZXRlcy5pbyI6eyJuYW1lc3BhY2UiOiJjbW8tYm9vdHN0cmFwIiwic2VydmljZWFjY291bnQiOnsibmFtZSI6ImNtby1ib290c3RyYXAtYWRtaW4iLCJ1aWQiOiJlY2Q1NGUzYS04M2I2LTRmNmMtYjAyNi1iZmUzZTBlODAzZTEifX0sIm5iZiI6MTY5OTc5Mzc3OCwic3ViIjoic3lzdGVtOnNlcnZpY2VhY2NvdW50OmNtby1ib290c3RyYXA6Y21vLWJvb3RzdHJhcC1hZG1pbiJ9.ApEzEdbklYwVfrvfWiYvningIZOxd3HAscgQmB5RuPu1FV4v_cX3GvLgONUb1NLSCEk1P_vEbrtUqbjT0LWjbPKQ-Bd6zVoeMwStAa_camEdcu6ZTOFtZe46FFp5TmwXqrJdU8GVb_oFm70ZQsg0f_WuuwngONPQgtaVzq3rSs_l_XOAnI9yVS2Zzc4YkY6aQQuCnRCgtsG7Hg0fPg3VbzMxnouNRRcfIZ6ywBiIPCpofmWplQERxkF2SEinc1h3CjQRMBYRUsGxd1d9zw38gEEXKpoPXKCfG1mDDh1Q3pehEu99cCq3ZuLioToKJsjoZF2LnsiywHgpeB07qbFOOg"

    //url := fmt.Sprintf("%s/apis/cmo.dell.com/v1/namespaces/%s/cmoprovisioningconfigs/powerscale-appliance", kubernetesAPIServer, namespace)
    url := fmt.Sprintf("%s/apis/cmo.dell.com/v1/namespaces/%s/cmoprovisioningconfigs/powerscale", kubernetesAPIServer, namespace)
    //fmt.Println("url:", url)

    // Create a new request using http
    req, err := http.NewRequest("GET", url, nil)
    if err != nil {
        panic(err)
    }

    token, err := GetBootstrapClusToken()
    if err != nil {
        panic(err)
    }
    //fmt.Println("token:", token)

    // Add authorization header to the req
    req.Header.Set("Authorization", "Bearer "+token)

    client := http.DefaultClient
    tr := &http.Transport{
        TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
    }
    client = &http.Client{Transport: tr}

    // Send req using http Client
    resp, err := client.Do(req)
    if err != nil {
        panic(err)
    }
    defer resp.Body.Close()

    // Read the response body
    body, err := ioutil.ReadAll(resp.Body)
    if err != nil {
        panic(err)
    }

    fmt.Println(string(body))
}

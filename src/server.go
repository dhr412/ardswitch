package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os/exec"
)

func runExecutable(path string) error {
	cmd := exec.Command(path)
	return cmd.Run()
}

func triggerHandler(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case http.MethodGet, http.MethodPost:
		err := runExecutable("autom.exe")
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			return
		}
		w.WriteHeader(http.StatusOK)

	default:
		w.WriteHeader(http.StatusMethodNotAllowed)
	}
}

func main() {
	serveIp := flag.String("ip", "0.0.0.0", "IP address to bind the server to")
	port := flag.Int("port", 3200, "Port to run the server on")
	flag.Parse()

	http.HandleFunc("/triggered", triggerHandler)

	addr := fmt.Sprintf("%s:%d", *serveIp, *port)
	fmt.Printf("Server running on http://%s:%d\n", *ip, *port)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}

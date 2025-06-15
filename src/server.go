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
	port := flag.Int("port", 3200, "Port to run the server on")
	flag.Parse()

	http.HandleFunc("/triggered", triggerHandler)

	addr := fmt.Sprintf(":%d", *port)
	fmt.Println("Server running on http://0.0.0.0" + addr)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}

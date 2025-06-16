package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"os/exec"
	"strings"
	"sync"
	"time"
)

const endpnt = "triggered"

var (
	requestCount int
	mu           sync.Mutex
)

func runExecutable(path string) error {
	cmd := exec.Command(path)
	return cmd.Run()
}

func triggerHandler(w http.ResponseWriter, r *http.Request) {
	mu.Lock()
	requestCount++
	currentCount := requestCount
	mu.Unlock()
	fmt.Printf("[%s] Server hit #%d\n", time.Now().Local().Format("15:04:05"), currentCount)

	switch r.Method {
	case http.MethodGet, http.MethodPost:
		err := runExecutable("./autom.exe")
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			fmt.Printf("Error running automation: %v\n", err)
			return
		}
		w.WriteHeader(http.StatusOK)
		fmt.Print("Automation run successfully\n\n")

	default:
		w.WriteHeader(http.StatusMethodNotAllowed)
	}
}

func getLocalIP() string {
	commands := []string{
		`Get-NetAdapter -Name "Wi-Fi*" | Get-NetIPAddress -AddressFamily IPv4 | Select-Object -ExpandProperty IPAddress`,
		`Get-NetAdapter -Name "Ethernet*" | Get-NetIPAddress -AddressFamily IPv4 | Select-Object -ExpandProperty IPAddress`,
	}

	for _, psCmd := range commands {
		cmd := exec.Command("powershell", "-Command", psCmd)
		output, err := cmd.Output()
		if err != nil {
			continue
		}

		lines := strings.Split(string(output), "\n")
		for _, line := range lines {
			ip := strings.TrimSpace(line)
			if strings.HasPrefix(ip, "192.168") {
				return ip
			}
		}
	}

	return ""
}

func main() {
	port := flag.Int("port", 32768, "Port to run the server on (default: 32768)")
	flag.Parse()
	if *port < 1 || *port > 65535 {
		log.Fatalf("Invalid port number: %d.\nMust be between 1 and 65535.", *port)
	}

	http.HandleFunc("/"+endpnt, triggerHandler)

	addr := fmt.Sprintf("0.0.0.0:%d", *port)
	fmt.Printf("Server running on http://%s:%d/%s\n", getLocalIP(), *port, endpnt)

	if err := http.ListenAndServe(addr, nil); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}

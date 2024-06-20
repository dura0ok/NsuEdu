package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"sync"
	"time"
)

func runCommand(url string, wg *sync.WaitGroup) {
	defer wg.Done()

	cmd := exec.Command("curl", "-x", "127.0.0.1:8080", "-o", "output", "-v", url)
	cmd.Stderr = os.Stderr

	err := cmd.Run()
	if err != nil {
		fmt.Printf("Error executing command for %s: %s\n", url, err)
	}
}

func copyElements(input []string, numCopies int) []string {
	result := make([]string, 0, len(input)*numCopies)
	for i := 0; i < numCopies; i++ {
		result = append(result, input...)
	}
	return result
}

func shuffle(input []string) []string {
	rand.Seed(time.Now().UnixNano())
	rand.Shuffle(len(input), func(i, j int) {
		input[i], input[j] = input[j], input[i]
	})
	return input
}

func main() {
	urls := []string{
		"http://212.183.159.230/30MB.zip",
		"http://212.183.159.230/301adsdasdMB.zip",
		"http://212.183.159.230/30MB.zip",
		"http://fit.ippolitov.me/",
	}

	numCopies := 5

	urlsCopied := copyElements(urls, numCopies)

	randomizedURLs := shuffle(urlsCopied)

	var wg sync.WaitGroup

	for _, url := range randomizedURLs {
		wg.Add(1)
		go runCommand(url, &wg)
	}

	wg.Wait()

	fmt.Println("All commands completed.")
}

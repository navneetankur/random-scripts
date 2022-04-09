package main

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"strings"
	"syscall"
	"time"
)

var println=fmt.Println
var fatBinDir = os.Getenv("HOME") + "/bin/"
var fatBinName = "goFatBin"
var fatScriptName = "goFatBin.go"
var fatScript = fatBinDir + "/scripts/" + fatScriptName
var fatBin = fatBinDir + fatBinName

func main() {
	err := os.MkdirAll(fatBinDir, 770)
	check(err)
	scriptInfo,err := os.Stat(fatScript)
	check(err)
	scriptTime := scriptInfo.ModTime();
	binInfo,err := os.Stat(fatBin)
	var binTime time.Time
	if err == nil {
		binTime = binInfo.ModTime()
	} else if os.IsNotExist(err) {
		binTime = scriptTime.Add(-1 * time.Hour)
	} else { panic(err) }
	if scriptTime.After(binTime) {
		//read script
		readFp,err := os.Open(fatScript)
		check(err)
		defer readFp.Close()
		scanner := bufio.NewScanner(readFp)
		tmpFile,exists := os.LookupEnv("XDG_RUNTIME_DIR")
		if exists {
			tmpFile = strings.TrimSuffix(tmpFile,"/")
		} else {
			tmpFile = "/tmp"
		}
		tmpFile = tmpFile + "/" + fatScriptName
		writeFp,err := os.Create(tmpFile)
		check(err)
		defer writeFp.Close()
		scanner.Scan() //firts line is shebang
		for scanner.Scan() {
			writeFp.WriteString(scanner.Text())
			writeFp.WriteString("\n")
		}
		cmd := "go"
		args := []string{ "build", "-o", fatBin, tmpFile}
		output,err := exec.Command(cmd, args...).CombinedOutput()
		check(err,string(output))
	}
	syscall.Exec(fatBin, os.Args, os.Environ())
}

func check(e error, output ... string) {
	if e != nil {
		println(output)
		panic(e)
	}
}

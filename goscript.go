package main

import (
	"bufio"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

var println=fmt.Println

var fatScriptDir = os.Getenv("HOME") + "/bin/scripts/"
var fatScriptName = "goFatBin.go"
var fatScript = fatScriptDir + fatScriptName
func main() {
	mainArg := os.Args[1]
	if mainArg == "edit" {
		edit()
	} else if mainArg == "link" {
		os.Symlink(fatScript, os.Args[2])
	} else {
		println("Unknown command:",os.Args[1:])
	}
}
func edit() {
	//copied from fatrunner
	tmpDir,exists := os.LookupEnv("XDG_RUNTIME_DIR")
	if exists {
		tmpDir = strings.TrimSuffix(tmpDir,"/")
	} else {
		tmpDir = "/tmp"
	}
	tmpFile := tmpDir + "/" + fatScriptName
	scriptFp,err := os.Open(fatScript)
	check(err)
	tempFp,err := os.Create(tmpFile)
	check(err)
	scanner := bufio.NewScanner(scriptFp)
	writer := bufio.NewWriter(tempFp)
	scanner.Scan() //first line is shebang
	for scanner.Scan() {
		// writer.WriteString(scanner.Text())
		writer.Write(scanner.Bytes())
		writer.WriteString("\n")
	}
	writer.Flush()
	scriptFp.Close()
	tempFp.Close()
	var editor = "vim"
	editor,err = exec.LookPath(editor)
	check(err,editor)
	//also put go.mod file for lsp
	gomodFp,err := os.Create(tmpDir + "/go.mod")
	check(err)
	gomodFp.WriteString("module example.com/m\n\ngo 1.16")
	gomodFp.Close()
	vimCmd := exec.Command(editor,tmpFile)
	vimCmd.Stdin = os.Stdin
	vimCmd.Stdout = os.Stdout
	vimCmd.Stderr = os.Stderr
	err = vimCmd.Run()
	check(err)
	//editing done write back the file
	scriptFp,err = os.Create(fatScript)
	check(err)
	tempFp,err = os.Open(tmpFile)
	check(err)
	scanner = bufio.NewScanner(tempFp)
	writer = bufio.NewWriterSize(scriptFp,4096)
	writer.WriteString("#!/usr/bin/env fatRunner\n")
	for scanner.Scan() {
		writer.Write(scanner.Bytes())
		writer.WriteString("\n")
	}
	writer.Flush()
	scriptFp.Close()
	tempFp.Close()
	err = os.Chmod(fatScript, 0770)
	check(err)
}
func check(e error, output ... string) {
	if(e != nil) {
		panic(e)
	}
}

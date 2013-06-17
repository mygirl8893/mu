function syscall-0
[int64 id]
[
	let nothing [asm unit syscall {%%%}{ax}%%% id]
]
[[;nothing]]

function syscall-1
[int64 id int64 arg1]
[
	let nothing [asm int64 syscall {%%%}={ax},{ax},{di}%%% id arg1]
]
[[int64 nothing]]

function syscall-3
[int64 id int64 arg1 int64 arg2 int64 arg3]
[
	let nothing [asm int64 syscall {%%%}={ax},{ax},{di},{si},{dx}%%% id arg1 arg2 arg3]
]
[[int64 nothing]]

function exit_linux
[int64 code]
[
	let nothing [syscall-1 cint64 #d60 code]
]
[[;nothing]]

function exit_osx
[int64 code]
[
	let nothing [syscall-1 cint64 #h2000001 code]
]
[[;nothing]]

function linux
[]
[]
[[int1 cint1 #0]]

function exit
[int64 code]
[
	let linux_l osx [if [linux]]
	let result [join 
		[exit_linux code; linux_l]
		[exit_osx code; osx]
	]
]
[[; result]]

function write-linux
[int64 file-descriptor ptr int8 data int64 size]
[
]
[[]]

function write-osx
[int64 file-descriptor ptr int8 data int64 size]
[
	let nothing [syscall-3 cint64 #h2000004 file-descriptor [ptrtoint data int64] size]
]
[[;nothing]]

function write
[int64 file-descriptor ptr int8 data int64 size]
[
	let linux_l osx [if [linux]]
	let result [join 
		[write-linux file-descriptor data size; linux_l]
		[write-osx file-descriptor data size; osx]
	]
]
[[;result]]

function write-test
[int64 fd]
[
	let text [alloca array int8 #13]
	let stored [store ascii 
{%}Hello world!
% text]
	let result [write fd [bitcast text ptr int8] cint64 #13; stored]
]
[[;result]]

function open-osx
[ptr int8 path int64 flags int64 mode]
[
	let fd [syscall-3 cint64 #h2000005 [ptrtoint path int64] flags mode]
]
[[int64 fd]]

function open-linux
[ptr int8 path int64 flags int64 mode]
[
	let fd [syscall-3 cint64 #h2000005 [ptrtoint path int64] flags mode]
]
[[int64 fd]]

function open
[ptr int8 path int64 flags int64 mode]
[
	let linux_l osx [if [linux]]
	let fd [join
		[open-osx path flags mode; osx]
		[open-linux path flags mode; linux_l]
	]
]
[[int64 fd]]

function entry
[]
[
	let text [alloca array int8 #26]
	let stored [store ascii /Users/clemahieu/test.txt:a00 text]
	let fd [open [bitcast text ptr int8] cint64 #h602 cint64 #0; stored]
	let write_l [write-test fd]
	let result [exit cint64 #0; write_l]
]
[[; result]]

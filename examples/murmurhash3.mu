function rotl32
[int32 x int32 r]
[
	let result [or [shl x r] [lshr x [sub cint32 #32 r]]]
]
[[int32 result]]

function rotl64
[int64 x int64 r]
[
	let result [or [shl x r] [lshr x [sub cint64 #64 r]]]
]
[[int64 result]]

function getblock32 
[ptr int32 p int32 i]
[
	let result [load [getelementptr p i]]
]
[[int32 result]]

function getblock64
[ptr int64 p int32 i]
[
	let result [load [getelementptr p i]]
]
[[int64 result]]

function fmix32
[int32 h]
[
	let h1 [xor h [lshr h cint32 #16]]
	let h2 [mul h1 cint32 #h85ebca6b]
	let h3 [xor h2 [lshr h2 cint32 #13]]
	let h4 [mul h3 cint32 #hc2b2ae35]
	let h5 [xor h4 [lshr h4 cint32 #16]]
]
[[int32 h5]]

function fmix64
[int64 k]
[
	let k1 [xor k [lshr k cint64 #33]]
	let k2 [mul k1 cint64 #hff51afd7ed558ccd]
	let k3 [xor k2 [lshr k2 cint64 #33]]
	let k4 [mul k3 cint64 #hc4ceb9fe1a85ec53]
	let k5 [xor k4 [lshr k4 cint64 #33]]
]
[[int64 k5]]

function murmurhash3_x86_32 
[ptr int32 key int32 len int32 seed ptr int32 out]
[
	let nblocks [udiv len cint32 #4]
	let kn 
		loop
		[[sub cint32 #0 nblocks] seed]
		[i h1]
		[
			let k1 [getblock32 key i]
			let k2 [mul k1 cint32 #hcc9e2d51]
			let k3 [rotl32 k2 cint32 #15]
			let h2 [xor h1 k3]
			let h3 [rotl32 h2 cint32 #13]
			let h4 [add [mul h3 cint32 #5] cint32 #he6546b64]
			let exit continue [if [icmp eq i cint32 #0]]
			let in [sub i cint32 #1]
		]
	[[in h4; continue][h4; exit]]
	let t3 t2 t1 [switch [and len cint32 #3] cint32 #3 cint32 #2 cint32 #1]
	let k1 [xor cint32 #0 [shl [load [getelementptr key cint32 #2]] cint32 #16]; t3]
	let k2 [join k1 [~ cint32 #0; t2]]
	let k3 [xor k2 [shl [load [getelementptr key cint32 #1]] cint32 #8]]
	let k4 [join k3 [~ cint32 #0; t1]]
	let k5 [xor k4 [load key]]
	let k6 [mul k5 cint32 #hcc9e2d51]
	let k7 [rotl32 k6 cint32 #15]
	let k8 [mul k7 cint32 #h1b873593]
	let hn [xor kn k8]
	let result1 [xor hn len]
	let result2 [fmix32 result1]
	let result3 [store result2 out]
]
[[; result3]]

function entry
[]
[]
[[; unit_v [rotl32 cint32 #8 cint32 #8] [rotl64 cint64 #16 cint64 #8]]]

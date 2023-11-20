file = open("FunBBCallFun.txt", "r+")

FunCalledNumDist = {}
for line in file:
	linelist = line.split(",")
	funName = linelist[2]
	FunCalledNumDist[funName]=0

file.close()

file = open("FunBBCallFun.txt","r+")
for line in file:
	linelist = line.split(",")
	funName = linelist[2]
	value = FunCalledNumDist[funName]
	value = value +1
	FunCalledNumDist[funName]=value

BBEntryDegreeDist = {}
for key in FunCalledNumDist:
	print(key)
	fileBBPreNum = open("Fun_BB_PreNum.txt")
	for line in fileBBPreNum:
		linelist = line.split(",")
		funName = linelist[0]
		if(key == funName):
			print(key)
			BB=linelist[1]
			print(BB)
			BBEntryDegreeDist[BB]=FunCalledNumDist[key]+linelist[2]

print(BBEntryDegreeDist)

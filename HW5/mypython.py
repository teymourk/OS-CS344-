import random 

#alphabet Characters
alphatbet = ['a', 'b','c','d','e','f','g','h',
			 'i','j','k','l','m','n','o','p',
			 'q','r','s', 't', 'u','v','w','x','y','z' ]
	
def alphabeCharacters():

	#create a list from all the letters and suffle it
	deck = list(range(0, len(alphatbet)))
	random.shuffle(deck)

	i = 0
	randomAlphabet = []
	fileteredLetters = ""
	#loop and insert 10 random ones in to an empty list
	#and remove the spaces
	while i < 10:

		index = deck[i]
		alpha = alphatbet[index]
	
		randomAlphabet.append(alpha)
		fileteredLetters = "".join(randomAlphabet)

		i += 1
		
	#return with new line charachter
	return str(fileteredLetters + '\n')

#put the content in the files and 
#close the file
file1Content = alphabeCharacters()
print("File 1 Contents: " + file1Content)
file1 = open('file1.txt', 'w+')
file1.write(file1Content)
file1.close()

file2Content = alphabeCharacters()
print("File 2 Contents: " + file2Content)
file2 = open('file2.txt', 'w+')
file2.write(file2Content)
file2.close()

file3Content = alphabeCharacters()
print("File 3 Contents: " + file3Content)
file3 = open('file3.txt', 'w+')
file3.write(file3Content)
file3.close()

def randomProduct():
	#create two random numbers between 1 and 42
	num1 = random.randint(1, 42)
	num2 = random.randint(1, 42)
	product = num1 * num2

	#print the product of the two
	print("Product of " + str(num1) + " and " + str(num2)  + "  is " + str(product))

randomProduct()
import speech_recognition as sr

print("The system will start recording after youn enter your chosen keyword to count. ")
print("Enter your chosen keyword: ")

ChosenKeyword = input()

# obtain audio from the microphone
r = sr.Recognizer()
with sr.Microphone() as source:
    print("Recoring in Progress::")
    audio = r.listen(source)

try:
    sentence = r.recognize_google(audio)
    words = sentence.split()
    print("YOUR TEXT: " + sentence)
    wordcounter = 0
    for word in words:
        if ChosenKeyword == word:
            wordcounter = wordcounter + 1
    print("Your Chosen word: " + ChosenKeyword + " was repeated: " + str(wordcounter) + " times.")
except:
    pass
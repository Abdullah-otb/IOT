import speech_recognition as sr

ChosenKeyword = input()

r = sr.Recognizer()

with sr.Microphone() as source:
    print("SAY SOMETHING")
    audio = r.listen(source)
    print("TIME OVER, THANKS")

try:
    sentence = r.recognize_google(audio)
    words = sentence.split()
    print("TEXT: " + sentence)
    counter = 0
    for word in words:
        if ChosenKeyword == word:
            counter = counter + 1
    print("Your Chosen word: " + ChosenKeyword + " was repeated: " + str(counter) + " times.")
except:
    pass

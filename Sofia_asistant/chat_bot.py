from openai import OpenAI

client = OpenAI(
    base_url = 'http://localhost:11434/v1',
    api_key='ollama', # required, but unused
)

ASSISTANT_PROMPT = """
You're my friendly assistance. 
When i have a request, start by calling me "Mr Boss".
Give quick answers unless I ask for an explanation. 
Never include <think></think>. Just answer concisely.
"""

messages = [{"role": "system", "content": ASSISTANT_PROMPT}]

while True:
#    print(messages)
    user_type = input("you: ")
    if user_type == "end chat":
        break

    messages.append({"role": "system", "content": user_type})

    response = client.chat.completions.create(
        model="deepseek-r1:8b",
        stream=True,
        messages=messages
    )

    bot_reply = ""
    for chunk in response:
        bot_reply += chunk.choices[0].delta.content
        print( chunk.choices[0].delta.content or "", end = "", flush=True)
    #bot_reply = response.choices[0].message.content
    #print(bot_reply)

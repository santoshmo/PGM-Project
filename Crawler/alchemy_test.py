import json
from alchemyapi import AlchemyAPI
alchemyapi = AlchemyAPI()

demo_url = "http://www.cse.buffalo.edu/~jcorso/"
response = alchemyapi.text('url', demo_url)

if response['status'] == 'OK':
    # print('## Response Object ##')
    # print(json.dumps(response, indent=4))

    # print('')
    # print('## Text ##')
    print(response['text'].encode('utf-8'))
    print('')
else:
    print('Error in text extraction call: ', response['statusInfo'])
#encoding:utf8
from flask import Flask
app = Flask(__name__)

@app.route('/')
def show_hello():
	return 'Hello World.\n'

if __name__ == '__main__':
	app.run('0.0.0.0')




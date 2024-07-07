@echo off
REM Create virtual environment
call python -m venv venv

REM Activate virtual environment
call venv\Scripts\activate

REM Install Flask
call pip install Flask

REM Install bson
call pip install bson

REM Install pymongo
call pip install pymongo

REM Set FLASK_APP environment variable
set FLASK_APP=app.py

REM Run Flask app
call flask run --host=0.0.0.0

REM Pause the terminal window
call pause

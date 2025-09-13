#include <iostream>
#include <conio.h>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

std::string showStatus(int i)
{
	switch (i)
	{
	    case 0:  return "Yet To Bat";
    	case 1:  return "not out";
    	case 2:  return "bowled";
    	case 3:  return "caught out";
    	case 4:  return "run out";
    	default: return "unknown";
    }
    return "unknown";
}

class Team
{
    std::string m_teamName;
    std::map<std::string, int> m_playerToIndex;
    std::vector<std::string> m_playerName;
    std::vector<int> m_playerRun;
    std::vector<int> m_playerStatus;
    std::vector<int> m_playerBall;
    int m_noOfPlayers = 11;
    int m_extraRun = 0;
		    
	public:
       	Team();

       	int init(void);

       	std::string getName(int i)
	    {
			return m_playerName[i];
		}

       	int getRun(const std::string& playerName)
	   	{
	   		return m_playerRun[m_playerToIndex[playerName]];
		}

       	int getStatus(const std::string& playerName)
		{
		   return m_playerStatus[m_playerToIndex[playerName]];
		}

       	int getExtra(void)
		{
			return m_extraRun;
		}

       	int getBall(const std::string& playerName)
		{
			return m_playerBall[m_playerToIndex[playerName]];
		}

       	std::string getTeamName()
		{
			return m_teamName;
		}

       	int getTotal(void);
       	
		bool checkValidPlayer(const std::string& playerName)
		{
			return std::find(m_playerName.begin(), m_playerName.end(), playerName)!=m_playerName.end();
		}

		void addRun(const std::string& playerName,int run);
       	
		void setStatus(const std::string& playerName,int status)
		{
			m_playerStatus[m_playerToIndex[playerName]]=status;
		}
       	
		void setOut(const std::string& outPlayer, int type, std::string& newPlayer);
       	
		void setBall(const std::string& playerName)
		{
			m_playerBall[m_playerToIndex[playerName]]++;
		}

       	void setExtra(int run)
		{
			m_extraRun+=run;
		}
};

/**** Default Contructor**********/
Team::Team()
{
	m_teamName = "";
	m_extraRun=0;
	m_noOfPlayers=0;
}

/***** Initialize Team**********/
int Team::init(void)
{
  	cout<<"Enter Team Name: ";
	cin>>m_teamName;
 	cout<<"No of players in the team: ";
	cin>>m_noOfPlayers;

	m_playerName.resize(m_noOfPlayers);
	m_playerRun.resize(m_noOfPlayers);
	m_playerStatus.resize(m_noOfPlayers);
	m_playerBall.resize(m_noOfPlayers);
	
	for(int i=0; i<m_noOfPlayers;i++)
	{
   		m_playerRun[i]=0;
		m_playerStatus[i]=0;
		m_playerBall[i]=0;
	}

  	cout<<"Enter Name of the Players.......\n";
	for(int i=0;i<m_noOfPlayers;i++)
   	{
	    cout<<"                  Player"<<i+1<<": ";
		std::string playerName;
	    cin>>playerName;
		m_playerName[i]= playerName;
		m_playerToIndex[m_playerName[i]]=i;
	    cout<<"\n\n";
	    m_playerRun[i]=0;
	    m_playerStatus[i]=0;
	}
	return m_noOfPlayers-1;
}

void Team::addRun(const std::string& playerName,int run)
{
  	m_playerRun[m_playerToIndex[playerName]] += run;
  	getTotal();
}

/***** Calculate & Get Total Run ****/
int Team::getTotal()
{	
  	int local_total=0;
  	for(int i=0;i<m_noOfPlayers;i++)
  	{
    	local_total += m_playerRun[i];    //Players Score
	}
	local_total += m_extraRun;            //Add Extra
  	return local_total;
}

void Team::setOut(const std::string& outPlayer,int type,std::string& newPlayer)
{
	m_playerStatus[m_playerToIndex[outPlayer]]=type;      //Current Player out	
	m_playerStatus[m_playerToIndex[newPlayer]]=1;         //New Player Not out
}

/**********my score***********/
class ScoreSheet
{
    string m_player1;
    string m_player2;
    int m_ballCount;
    int m_over;
    int m_maxOver;
    int m_out;
    int m_maxOut;
    Team m_team1;
    int extra;
	public:
    	ScoreSheet();
    	void show(void);
    	void chooseOption(void);
    	void dotBall(void);
    	void addRun(void);
    	void wicket(void);
    	void extraRun(void);
    	void overComplete(void);
		void isEndChanged();
};

ScoreSheet::ScoreSheet()
{
	m_player1="";
	m_player2="";
	extra=0;
	m_over=0;
	m_out=0;
	m_ballCount=0;
	m_maxOut=m_team1.init();                            // Initialize Team
	cout<<"\nEnter no. of Overs in one Innings: ";
	cin>>m_maxOver;
	cout<<"\n\nChoose opening Batsman:";
	
	while(1)
	{
    	cout<<"\n                     batsman 1:";       //Chose
    	cin>>m_player1;                                  //Opening											
    	cout<<"\n                     batsman 2:";       //Batsman
    	cin>>m_player2;
    	
    	if(!m_team1.checkValidPlayer(m_player1) || !m_team1.checkValidPlayer(m_player2) || m_player1==m_player2)
      	{	
			cout<<"Invalid Entries Try Again!!!";
		}
    	else 
		{
			m_team1.setStatus(m_player1,1);
			m_team1.setStatus(m_player2,1);
			break;
		}
    }
}

/******* Showing Entire Score *********/
void ScoreSheet::show(void)
{	
	cout<<"//////////////////////////////////////////////////"<<endl;
	cout<<"                   "<<m_team1.getTeamName()<<" Score                     "<<endl;
	cout<<"//////////////////////////////////////////////////"<<endl<<endl;
	
	for(int i=0;i<=m_maxOut;i++)
    {
		std::string playerName = m_team1.getName(i);
		if(m_team1.getStatus(playerName)!=0)
		{
			std::string dspName = playerName;
			dspName += ((m_over < m_maxOver) && (m_out < m_maxOut) && (m_player1==playerName)) ? "*" : "";
			cout << dspName;
			for(int i=0; i<(32-dspName.size()); i++)
			{
				cout<<" ";
			}
			std::string status = showStatus(m_team1.getStatus(playerName));
			cout<<status; 
			for(int i=0; i<(27-status.size()); i++)
			{
				cout<<" ";
			}  				
			cout<<m_team1.getRun(playerName);
			cout<<"("<<m_team1.getBall(playerName)<<")";
			cout<<endl;
		}		
   	}
   	
	for(int i=0;i<=m_maxOut;i++)
    {
		std::string playerName = m_team1.getName(i);
		if(m_team1.getStatus(playerName)==0)
		{
			cout << playerName;
			for(int i=0; i<(32-playerName.size()); i++)
			{
				cout<<" ";
			}
			cout<<showStatus(m_team1.getStatus(playerName));
			cout<<endl;  	
		}		
    	
   	}

	cout<<"\nExtra"; 
	cout<<"\t"<<m_team1.getExtra()<<endl<<endl;
	cout<<"--------------------------------------------------"<<endl;
	cout<<"Over "<<m_over<<"."<<m_ballCount<<"  Wicket "<<m_out;
	cout<<" || total score: ";
	cout<<m_team1.getTotal()<<endl;
    
    if(m_over<m_maxOver && m_out<m_maxOut)
    {
    	chooseOption();
	}
}

void ScoreSheet::chooseOption()
{
	cout<<"\nChoose option:\n";                //Options for
  	cout<<"\n\t 1. Dot Ball";                  //Updating
 	cout<<"\n\t 2. Add Run";                   //Score Card
 	cout<<"\n\t 3. Extra";
  	cout<<"\n\t 4. Wicket";
  	cout<<"\n\t10. Exit\n";
  	cout<<"\t\t\t.................";
	int option;
	cin>>option;
	
	switch (option)
	{
    	case 1:    
			dotBall(); 
			break;
    	case 2:
		    addRun();  
			break;
    	case 3:
		    extraRun();  
			break;
    	case 4:
		    wicket();  
			break;
    	case 10:  
			return;
    	default:
			cout<<"\nInvalid input\n"; 
	 
		dotBall();
    }
}

/****** One Dot Ball *******/
void ScoreSheet::dotBall(void)
{
	cout<<"\n\n************\n";
	cout<<"             Dot Ball              \n";
	cout<<"************\n\n";
	m_ballCount++;
	m_team1.setBall(m_player1);
	
	if(m_ballCount==6)
	{
		overComplete();
		return;
	}
	
	show();
}

void ScoreSheet::isEndChanged()
{
	cout<<"End changed(y/n)?\n";
	std::string endChanged;
	cin>>endChanged;

	while(endChanged !="y" && endChanged !="n")
	{
		cout<<"Please enter 'y' or 'n': ";
		cin>>endChanged;
	}

	if(endChanged=="y")
	{
		std::string tmp = m_player1;
		m_player1 = m_player2;
		m_player2 = tmp;
	}
} 

/****** Add Current Player Run********/
void ScoreSheet::addRun(void)
{
	cout<<"Runs? ";
	int runs;
	m_ballCount++;
	m_team1.setBall(m_player1);
	cin>>runs;
	m_team1.addRun(m_player1,runs);
	
	if(runs==1||runs==3 || runs==5)
	{
	    std::string temp = m_player1;
	    m_player1=m_player2;
	    m_player2=temp;
	}
	
	if(m_ballCount==6)
	{
		overComplete();
		return;
	}
	
	show();
}

/****** Add Extra Run ********/
void ScoreSheet::extraRun(void)
{
	cout<<"\n\n************\n";
	cout<<"             Extra Run              \n";
	cout<<"************\n\n";
	cout<<"Extra Type?"<<std::endl;
	cout<<"1. Wide"<<std::endl;
	cout<<"2. No Ball"<<std::endl;
	cout<<"3. Leg Byes"<<std::endl;
	cout<<"4. Byes"<<std::endl;
	cout<<"5. No Ball & Leg Byes"<<std::endl;
	cout<<"6. No Ball & Byes"<<std::endl;
	cin>>extra;
	
	if(extra == 1)
	{
		cout<<"Runs? "<<std::endl;
		int runs;
		cin>>runs;
		m_team1.setExtra(runs);
		isEndChanged();
	}
	else if(extra == 3 || extra == 4)
	{
		m_ballCount++;
	    m_team1.setBall(m_player1);
		cout<<"Runs? "<<std::endl;
		int runs;
		cin>>runs;
		m_team1.setExtra(runs);
		isEndChanged();
	}
	else if(extra == 2)
	{
		cout<<"Only put runs scored by player";
		addRun();
		m_ballCount--; // removing extra ball counted in addRun as legal delivery
		m_team1.setExtra(1);
	}
	else if(extra == 5 || extra == 6)
	{
		m_team1.setBall(m_player1);
		cout<<"Runs? "<<std::endl;
		int runs;
		cin>>runs;
		m_team1.setExtra(runs);
		isEndChanged();
	}

	show();
}

/****** Over Complete **********/
void ScoreSheet::overComplete(void)
{
	cout<<"\n\n*************\n";
	cout<<"           Over Complete\n";
	cout<<"***********\n";
	m_over++;
	m_ballCount=0;
	std::string temp = m_player1;
	m_player1=m_player2;
	m_player2=temp;
	
	if(m_over==m_maxOver)
	{
		cout<<"\n\n*************\n";
		cout<<"           Innings Complete\n";
		cout<<"***********\n";
		show();
		return;
	}	
	else 
	{
		show();
	}	
}

/******** Wicket Fallen ********/
void ScoreSheet::wicket(void)
{
	int oType;
	std::string newPlayer;
	cout<<"\n\n*************\n";
	cout<<"           Wicket\n";
	cout<<"***********\n";
	cout<<"\nOut type? (Bold-1:Caught-2:Run_out:3).......";
	cin>>oType;
	m_out++;
	m_ballCount++;
	m_team1.setBall(m_player1);
	
	if(m_out>=m_maxOut)
	{
		m_team1.setStatus(m_player1,oType+1);
		cout<<"\n\n*************\n";
		cout<<"           Innings Complete\n";
		cout<<"***********\n";
		show();
		return;
	}
	
	cout<<"\nNew Batsman: ";
	cin>>newPlayer;

	while(m_team1.getStatus(newPlayer)>0 || !m_team1.checkValidPlayer(newPlayer))
	{
		cout<<"!!! Add valid player !!!"<<std::endl;
		cout<<"\nNew Batsman: ";
		cin>>newPlayer;
	}

	if(oType == 3)
	{
		cout<<"Completed ";
		int runs;
		m_team1.setBall(m_player1);
		cin>>runs;
		m_team1.addRun(m_player1,runs);
		cout<<"Player got out?";
		std::string outPlayer;
		cin>>outPlayer;
		while(outPlayer!=m_player1 || outPlayer!=m_player2)
		{
			std::cout<<"!!!Invalid player name!!!"<<std::endl;
			cout<<"Player got out?";
			cin>>outPlayer;
		}

		std::string strikePlayer;
		cout<<"Player on Strike(Ignore change of ends if over completed)?";
		cin>>strikePlayer;

		while(!(strikePlayer==newPlayer || strikePlayer==m_player2 ))
		{
			std::cout<<"!!!Invalid player name!!!"<<std::endl;
			cout<<"Player on Strike(Ignore change of ends if it's over completed)?";
			cin>>strikePlayer;
		}

		std::string stillNotOutPlayer = outPlayer==m_player1?m_player2:m_player1;			
		m_player1 = strikePlayer;
		m_player2 = strikePlayer==stillNotOutPlayer?newPlayer:stillNotOutPlayer;			
	}
	else
	{
		m_team1.setOut(m_player1,oType+1,newPlayer);
		m_player1=newPlayer;
	}
	
	if(m_ballCount==6)
	{
		overComplete();
		return;
	}
	
	show();
}

int main(void)
{
	cout<<"///////////////////////////////////////////////////////////////////////"<<endl;
	cout<<"                        Cricket Score Sheet App\n";
	cout<<"           Tested by Best Straight Drive Shot Player of VSCA\n";
	cout<<"///////////////////////////////////////////////////////////////////////"<<endl;
	ScoreSheet score;
	score.show();

}


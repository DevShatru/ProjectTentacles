# AutoBuild 
Instructions for setting up Scheduled daily builds using our `AutoBuild.cmd` script
## Setup
- Open _Task Scheduler_ (Built-in Windows Utility)<br/>
- In the right-most column of the scheduler (under _Actions_), select _Create Basic Task_<br/>
- This should open a _Create Basic Task Wizard_ window, fill in _Name_ and _Description_ however you like and hit Next

- For Trigger:<br/>
i) Select _Daily_ and hit Next<br/>
ii) Set up the _Start_ date and time as you like (No need to check _Synchronize across time zones_)<br/>
iii) Leave _Recur every: 1 days_ (Recommend running it daily even if you only check it 2/3 times a week), and hit Next

- Leave Action on _Start a program_
- In Start a Program:<br/>
i) Browse to the script<br/>
ii) For _Add arguments (optional):_ you have two options<br/>
    &ensp;a) Leave it blank, build will be placed in the same directory as the project in a _Release_ folder <br/>
    &ensp;b) Use `--release:network` to build to your Desktop (`C:\Users\<YourUser>\Desktop`)<br/>
iii) (__IMPORTANT YOU NEED TO FILL THIS OR THE SCRIPT WON'T RUN__) For _Start in (optional):_ (__IMPORTANT YOU NEED TO FILL THIS OR THE SCRIPT WON'T RUN__) Fill in the directory the project is in (Ex: In my case it was `C:\Users\shatr\Documents\ProjectTentacles`)

- On the _Finish_ tab you can review your setup and click _Finish_

## Testing
You can manually run the script from _Task Scheduler_ to ensure it is setup properly
- Back on the Task Scheduler, select _Task Scheduler Library_ from the left-most column<br/>
- Select your Task from the list (_Name_ will be the name you just created, and _Author_ will be your user) <br/>
- In the Actions tab on the right (under _Selected Item_), click _Run_ to execute the script and ensure the task is running properly
- This will popup a Command Prompt window displaying debug information while the script runs (It should automatically close when the script is finish)
- Check the build your setup release folder (Either Desktop or Project directory)
- 
## Offline builds
By default the scheduler only runs if you are logged in to your account. You can set it up to run even if you are logged out/your PC is sleeping with the following steps:
- In the _Task Scheduler Library_, right-click your task and select _Properties_
- In the _General_ tab (under _Security options_) select _Run whether user is logged on or not_ and hit Okay
- A window will open prompting you to enter the password for your user (Perfectly safe, this is a windows utility), hit Okay after doing so
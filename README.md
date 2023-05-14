# CSE109 - Systems Software - Spring 2023

# Final Exam

**Due Date: 5/14/23 EOD **

## Ethics Contract

**FIRST**: Please read the following carefully:

- I have not received, I have not given, nor will I give or receive, any assistance to another student taking this exam, including discussing the exam with students in another section of the course. Do not discuss the exam after you are finished until final grades are submitted.
- If I use any resources (including text books and online references and websites), I will cite them in my assignment.
- I will not ask question about how to debug my exam code on Piazza or any other platform.
- I will not plagiarize someone else's work and turn it in as my own. If I use someone else's work in this exam, I will cite that work. Failure to cite work I used is plagiarism.
- I understand that acts of academic dishonesty may be penalized to the full extent allowed by the [Lehigh University Code of Conduct][0], including receiving a failing grade for the course. I recognize that I am responsible for understanding the provisions of the Lehigh University Code of Conduct as they relate to this academic exercise.

If you agree with the above, type your full name in the following space along with the date. Your exam **will not be graded** without this assent. When you are done, **make your first commit with the commit message: `I, <your full name here>, agree to the ethics contract`.**

Write your name and date between the lines below

---------------------------------------------

---------------------------------------------

## Exam Prelude

**Read thoroughly before starting your exam:**

### Instructions 

1. Fork this repository into your CSE109 project namespace. [Instructions](https://docs.gitlab.com/ee/workflow/forking_workflow.html#creating-a-fork)
2. Clone your newly forked repository onto your development machine. [Instructions](https://docs.gitlab.com/ee/gitlab-basics/start-using-git.html#clone-a-repository) 
3. As you are writing code you should commit patches along the way. *i.e.* don't just submit all your code in one big commit when you're all done. Commit your progress as you work. **You should make at least one commit per question.**
4. When you've committed all of your work, there's nothing left to do to submit the assignment.

### Commit Policy

This exam is divided into discrete "questions", and you must make at least one commit per question. This is how I will know you are not just copying and pasting an entire solution from the internet. I will be able to see how much time you spend on each question based on your commit times. Red flags are raised when certain patterns of behavior indicating cheating are exhibited by your commit activity. 

You can do the questions in any order, and you can go back to questions for which you have already made a commit, but I want you to have at least one commit per question. The commit message for each question should be "QUESTION N" where N is the number of the question.

### Resource Usage Policy

You are free to use any resource for this exam. This includes books, notes, lecture videos, documentation, the internet, Stack Overflow, etc. The only course resources off limits are me and your TAs. I am free to answer clarifying questions, but that's it. I can't help you debug or provide any technical support (this includes support with git. Knowing how to use git is part of this exam.)

If you use any resources from the internet or anywhere else, **YOU HAVE TO CITE THEM** in your exam. Failure to cite resources you used may lead to failure of the exam. This includes the wiki article and videos I posted above. If you use them, cite them every time you use them. See the Works Cited section at the bottom for more details on how to cite your work in this document using Markdown.

## Assignment

For this exam, you will be writing the server half of a client/server pair that will communicate over Unix sockets. The pair is:

1. File Server - A file server is a program that hosts files for clients. It receives files that clients want to store, and sends them back to the client (or other clients) when they are requested. This is what you will be writing for this assignment. A compiled file server binary has been provided for you in the `bin` directory. You can use it as a reference for this exam.

2. Client - This program connects to the server and can send files to it, which will be stored on the file server. The client can also request files from the file server. A compiled client binary has been provided for you in the `bin` directory as well.

## File Server

The file server program must accept the following flag:

1. `--hostname address:port` - Where `address` is a 32 bit IP address, and `port` is the desired port of the file server.

If you call the program without a `--hostname`, it should use the default of `localhost:8081`.

---

- Q1. The server will start, bind a socket to a port, and listen for a connection.
- Q2. The server will create an empty hash map to store received files in memory.
  - You have to write the hash map yourself. You can't use a library that you did not write. You can use your Homework 6 code or the posted solution as a starting point. Remember to cite this code if you use it. If you look at any source for help, be sure to cite it.
  - The hash map will use the filename string as a key, and the file as a value (this can be either the received `File`, or the file data `vector<u8>`).
  - The hashmap will act much like the hash set, except it will have two additional methods:
    - Q3. `bool insert(String key, File value);` - inserts the k/v pair into the hash map. Returns true if the key already existed, and replaces the stored value with the supplied value. Returns false if the key did not exist already.
    - Q4. `File get(String key);` - Returns the file associated with the supplied key. If the key doesn't exist, this function throws an exception.
- When the server receives a connection, it will enter an infinite loop.
  - This loop will attempt to receive data from the client. When it receives a message, it will follow the following steps:
    - Q5. Read the message to a buffer
    - Q6. Decrypt the message using a simple XOR encryption scheme. The key is `42`.
    - Q7. Deserialize the message to the appropriate struct, either a `File` or `Request`.
      - Q8. If the message is a `File`, then the server will insert the filename and file into the hash map.
      - Q9.If the message is a `Request`, then the server will look for the requested file in hash map. 
        - If the requested file does not exist, nothing will be sent back to the client.
        - Q10. If the file does exist it will be serialized into a `File` message, encrypted, and sent to the client.
  - After servicing this message, the file server will loop and wait for a new message from the client.
  - The file server will not terminate until the user terminates the program or the client terminates the connection.

## Message Protocol

### Serialized File

If you have a file called `file.txt` with the contents `Hello`, then the serialzied, unencrypted message should look like this:

```
┌───────────────────────────────────────────────────────────────────────────────┐
│0xae     // map tag                                                            │
│0x01     // 1 kv pair                                                          │
├───────────────────────────────────────────────────────┬───────────┬───────────┤
│0xaa     // string8 tag                                │           │           │
│0x04     // 4 characters                               │ key       │           │
│File     // the string "File"                          │           │           │
├───────────────────────────────────────────────────────┼───────────┤  pair 1   │
│0xae     // the value associated with the key is a map │           │           │
│0x02     // 2 kv pairs                                 │           │           │
├────────────────────────────────┬───────────┬──────────┤           │           │
│0xaa     // string8 tag         │           │          │           │           │
│0x04     // 4 characters        │ key       │          │           │           │
│name     // the string "name"   │           │          │           │           │
├────────────────────────────────┼───────────┤ pair 1   │           │           │
│0xaa     // string8 tag         │           │          │           │           │
│0x08     // 8 characters        │ value     │          │           │           │
│file.txt // the filename        │           │          │           │           │
├────────────────────────────────┼───────────┼──────────┤           │           │
│0xaa     // string8 tag         │           │          │ value     │           │
│0x05     // 5 characters        │ key       │          │           │           │
│bytes    // the string "bytes"  │           │          │           │           │
├────────────────────────────────┼───────────┤ pair 2   │           │           │
│0xac     // array8 tag          │ value     │          │           │           │
│0x05     // 5 elements          │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│H        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│e        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│l        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│l        // a byte              │           │          │           │           │
|0xa2     // unsigned byte tag   │           │          │           │           │
│o        // a byte              │           │          │           │           │
└────────────────────────────────┴───────────┴──────────┴───────────┴───────────┘           
```

In decimal:
```
[174, 1, 170, 4, 70, 105, 108, 101, 174, 2, 170, 4, 110, 97, 109, 101, 170, 8, 102, 105, 108, 101, 46, 116, 120, 116, 170, 5, 98, 121, 116, 101, 115, 172, 5, 162, 72, 162, 101, 162, 108, 162, 108, 162, 111]
```

In hex:

```
[0xAE, 0x01, 0xAA, 0x04, 0x46, 0x69, 0x6C, 0x65, 0xAE, 0x02, 0xAA, 0x04, 0x6E, 0x61, 0x6D, 0x65, 0xAA, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74, 0xAA, 0x05, 0x62, 0x79, 0x74, 0x65, 0x73, 0xAC, 0x05, 0xA2, 0x48, 0xA2, 0x65, 0xA2, 0x6C, 0xA2, 0x6C, 0xA2, 0x6F]
```

### Serialized Request

If you have a file called `file.txt` that you are requesting, then the unencrypted message should look like this:

```
┌───────────────────────────────────────────────────────────────────────────────┐
│0xae     // map tag                                                            │
│0x01     // 1 kv pair                                                          │
├───────────────────────────────────────────────────────┬───────────┬───────────┤
│0xaa     // string8 tag                                │           │           │
│0x07     // 7 characters                               │ key       │           │
│Request  // the string "Request"                       │           │           │
├───────────────────────────────────────────────────────┼───────────┤  pair 1   │
│0xae     // the value associated with the key is a map │           │           │
│0x01     // 1 kv pair                                  │           │           │
├──────────────────────────────┬───────────┬────────────┤           │           │
│0xaa     // string8 tag       │           │            │           │           │
│0x04     // 4 characters      │ key       │            │ value     │           │
│name     // the string "name" │           │            │           │           │
├──────────────────────────────┼───────────┤ pair 1     │           │           │
│0xaa     // string8 tag       │           │            │           │           │
│0x08     // 8 characters      │ value     │            │           │           │
│file.txt // the filename      │           │            │           │           │
└──────────────────────────────┴───────────┴────────────┴───────────┴───────────┘           
```

In decimal:
```
[174, 1, 170, 7, 82, 101, 113, 117, 101, 115, 116, 174, 1, 170, 4, 110, 97, 109, 101, 170, 8, 102, 105, 108, 101, 46, 116, 120, 116]
```

In hex:

```
[0xAE, 0x01, 0xAA, 0x07, 0x52, 0x65, 0x71, 0x75, 0x65, 0x73, 0x74, 0xAE, 0x01, 0xAA, 0x04, 0x6E, 0x61, 0x6D, 0x65, 0xAA, 0x08, 0x66, 0x69, 0x6C, 0x65, 0x2E, 0x74, 0x78, 0x74]
```


## Video Explanation

This is the oral portion of the exam. You will record an explanation for your file server, and demonstrate that it works with a provided client.

To demonstrate your file server working, it's sufficient to show the client sending a file (any file) to the server, the server acknowledging its receipt, and then to show the client requesting it again and saving it to disk.

If you didn't get the file server fully working, explain how you attempted to solve this exam and where you got stuck. Show off any code you did write. This will get you full credit for this portion.

Here are some questions to answer during your video:

1. How did you handle errors and exceptions in your file server? Did you consider all possible error scenarios and how did you ensure that your file server gracefully handles them?

2. Did you consider the performance and efficiency of your file server in terms of file transfer speed and resource utilization? If so, what optimization techniques did you use to improve the performance of your file server? If not, what measures could you take to improve the efficiency of your file server?

3. Did you consider the possibility of the client sending malformed or malicious data to your file server? If so, what steps did you take to validate the data and prevent security vulnerabilities? If not, what measures could you take to improve the security of your file server?

You can use Zoom to do this, [here is a link][3] to some instructions. You don't have to record your face, only your voice and the screen. Go through your code and explain how you the important parts (important is subjective here. Usually the important bits are the ones you spent the most time on or had the most difficulty with). Your goal with this section is to convince me you know what you are talking about, so I want you to do this without reading a script or written answer. When you are done, upload your recording to your final exam repository.

Recording Link(s):

## Works Cited

Include a list of websites and resources you used to complete this exam. Make a numbered list, and at the place where you used this resource cite it using [reference style links in Markdown][1] (See this Readme's source for an example. It's invisible in the rendered document, but there are a number of links below this paragraph).

[0]: https://studentaffairs.lehigh.edu/content/code-conduct
[1]: https://www.markdownguide.org/basic-syntax#reference-style-links
[2]: http://crasseux.com/books/ctutorial/Building-a-library.html#Building%20a%20library
[3]: https://support.zoom.us/hc/en-us/articles/201362473-Local-recording

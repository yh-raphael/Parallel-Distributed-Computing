/*
 * Written by Raphael.
 * Creation Date : 2022-12-13-Tue.
 * Last Modification Date : 2022-12-13-Tue.
 */

package kafka;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.Collections;
import java.util.Properties;

import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;

import org.apache.kafka.clients.admin.*;
import org.apache.kafka.clients.admin.AdminClientConfig;
import org.apache.kafka.clients.admin.NewTopic;
import org.apache.kafka.clients.admin.CreateTopicsResult;
import org.apache.kafka.clients.admin.ListTopicsResult;
// import org.apache.kafka.common.config.TopicConfig;
import org.apache.kafka.common.PartitionInfo;
import org.apache.kafka.common.KafkaFuture;

import java.util.*;          // for Data Structures: Map, List, Set
import java.time.Duration;


// Consumer Class definition.
class Consumer
{
    KafkaConsumer<String, String> consumer;

    public void print_chat_room_list ()
    {
        Map <String, List<PartitionInfo> > topics = this.consumer.listTopics ();
        Set <String> keySet = topics.keySet ();
        for (String key : keySet)
        {
            if (key.equals ("__consumer_offsets")) continue;
            System.out.println (key);
        }
    }

    public void read ()
    {
        ConsumerRecords<String, String> records = this.consumer.poll (Duration.ofMillis (100)); 
        this.consumer.commitSync ();                                        // Synchronous Commit! Remember the position.
        for (ConsumerRecord<String, String> record : records)
		{
            System.out.printf ("%s: %s\n", record.key(), record.value());
	    }
    }

    public void subscribe (String topicName)
    {
        this.consumer.subscribe (Collections.singletonList (topicName));
    }

    public void reset_offset ()
    {
        this.consumer.seekToBeginning (this.consumer.assignment ());
    }
    
    public void close ()
    {
        this.consumer.close ();
    }

    // Creator!
    public Consumer (String ID)
    {
        Properties config = new Properties();                           // set the config.

        config.put(ConsumerConfig.GROUP_ID_CONFIG, ID);
        config.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
        config.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringDeserializer");
        config.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringDeserializer");
        config.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG,"earliest");
        config.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "false");

        this.consumer = new KafkaConsumer<>(config);                    //  member create?
    }
}

// Producer Class Definition.
class Producer
{
    KafkaProducer<String, String> producer;

    public void send (String room_name, String speaker, String mention)
    {
        ProducerRecord<String, String> record = new ProducerRecord<>(room_name, speaker, mention);
        producer.send (record);      // send method: producer record -> broker에 전송할 message (event)
    }

    public void close ()
    {
        this.producer.close ();     // close.
    }

    public Producer (String ID)
    {
        Properties config = new Properties();   // Properties -> producer 설정 정보 저장.

        config.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");                 // broker 목록 등.
        config.put(ProducerConfig.CLIENT_ID_CONFIG, ID);
        config.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringSerializer");       // key serializer
        config.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringSerializer");     // value serializer
        config.put(ProducerConfig.LINGER_MS_CONFIG, 1);     // producing lingering time.

        producer = new KafkaProducer<>(config);
    }
}

// Main Class.
public class SogangTalk
{
    static String ID;               // Should be global in the class.
    static Producer producer;
    static Consumer consumer;

    // [1] Window 1 - Login Window.
    public static void login_window () throws IOException
    {
        System.out.printf ("\n");
        System.out.printf ("Welcome to SogangTalk \n");
        System.out.printf ("1. Log in \n");
        System.out.printf ("2. Exit \n");
        System.out.printf ("\n");

        boolean prompt = true;
        BufferedReader br = new BufferedReader (new InputStreamReader (System.in));
        while (prompt)
        {
            System.out.printf ("SogangTalk> "); String select = br.readLine ();            
            switch (select)
            {
            case "1":   // Log in
                System.out.printf ("SogangTalk> ID: ");
                ID = br.readLine ();    // assign an ID.

                chatting_window ();     // go to the Window 2.

                System.out.printf ("\n");
                System.out.printf ("Welcome to SogangTalk \n");
                System.out.printf ("1. Log in \n");
                System.out.printf ("2. Exit \n");
                System.out.printf ("\n");
                break;

            case "2":   // Exit
                System.out.printf ("Bye! \n");
                prompt = false; // System.out.printf ("%s \n", prompt);
                break;
            }
        }
    }

    // [2] Window 2 - Chatting Window.
    public static void chatting_window () throws IOException
    {
        consumer = new Consumer (ID);   // Now, I'm gonna consume this Topic (chat room)!

        System.out.printf ("\n");
        System.out.printf ("Chatting Window \n");
        System.out.printf ("1. List \n");
        System.out.printf ("2. Make \n");
        System.out.printf ("3. Join \n");
        System.out.printf ("4. Log out \n");
        System.out.printf ("\n");

        boolean prompt = true;
        BufferedReader br = new BufferedReader (new InputStreamReader (System.in));
        while (prompt)
        {
            System.out.printf ("SogangTalk> "); String select = br.readLine ();  
            switch (select)
            {
            case "1":   // List
                consumer.print_chat_room_list ();
                break;

            case "2":   // Make
                System.out.printf ("SogangTalk> Chat room name: ");
                String room_name = br.readLine ();    // room_name -> Topic!

                // Create the New Topic which is Chat room name.
                Properties properties = new Properties ();
                properties.put (AdminClientConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092");
                try (Admin admin = Admin.create (properties))
                {
                    String topicName = room_name;       // added on the template.
                    int partitions = 1;                 // num of partitions: 1.
                    short replicationFactor = 1;        // num of replicas: 1.
                    NewTopic newTopic = new NewTopic (topicName, partitions, replicationFactor);
                    
                    CreateTopicsResult result = admin.createTopics (
                      Collections.singleton (newTopic)
                    );
                
                    KafkaFuture<Void> future = result.values ().get (topicName);
                    future.get();
                } catch (Exception e) { }
                // Creat session.

                System.out.printf ("\"" + room_name + "\"" + " is created!\n");
                break;

            case "3":   // Join
                System.out.printf ("SogangTalk> Chat room name: ");
                String join_room = br.readLine ();

                producer = new Producer (ID);       // Now, I'll be the producer for this Topic (chat room)!
                consumer.subscribe (join_room);     // Now, I'm gonna subscribe this Topic as well (chat room)!

                chat_room_window (join_room);

                System.out.printf ("\n");
                System.out.printf ("Chatting Window \n");
                System.out.printf ("1. List \n");
                System.out.printf ("2. Make \n");
                System.out.printf ("3. Join \n");
                System.out.printf ("4. Log out \n");
                System.out.printf ("\n");
                break;

            case "4":   // Log out
                consumer.close ();                  // Now, I'll stop the consuming!
                ID = null;              // To assign new users in the future.

                prompt = false;
                break;
            }            
        }
    }

    // [3] Window 3 - Chat Room Window.
    public static void chat_room_window (String current_room) throws IOException
    {
        System.out.printf ("\n");
        System.out.printf ("%s \n", current_room);
        System.out.printf ("1. Read \n");
        System.out.printf ("2. Write \n");
        System.out.printf ("3. Reset \n");
        System.out.printf ("4. Exit \n");
        System.out.printf ("\n");

        boolean prompt = true;
        BufferedReader br = new BufferedReader (new InputStreamReader (System.in));
        while (prompt)
        {
            System.out.printf ("SogangTalk> "); String select = br.readLine ();            
            switch (select)
            {
            case "1":   // Read.
                consumer.read();
                break;

            case "2":   // Write.
                System.out.printf ("SogangTalk> Text: ");
                String message = br.readLine ();

                producer.send (current_room, ID, message);
                break;

            case "3":   // Reset.
                consumer.reset_offset ();
                break;

            case "4":   // Exit.
                producer.close ();      // Now, I'll stop the producing!

                prompt = false;
                break;
            }
        }
    }

    // The main starting point!
	public static void main(String[] args) throws IOException
    {
        login_window ();
	}
}

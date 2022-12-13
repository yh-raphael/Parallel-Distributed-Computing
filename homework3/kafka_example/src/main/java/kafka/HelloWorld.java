package kafka;

import java.util.Collections;
import java.util.Properties;

import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;


public class HelloWorld {

    public static void producer(){
        Properties config = new Properties();   // Properties -> producer 설정 정보 저장.
        config.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:10400");                 // broker 목록 등?
        config.put(ProducerConfig.CLIENT_ID_CONFIG, "id1");
        config.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringSerializer");       // key serializer
        config.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringSerializer");     // value serializer
        config.put(ProducerConfig.LINGER_MS_CONFIG, 1);                                                                         // 전송 대기시간: 1ms 에 한 번씩.
        KafkaProducer<String, String> producer = new KafkaProducer<>(config);
        ProducerRecord<String, String> record = new ProducerRecord<>("topic_name", "12", "message_example");    // record 생성 방법 1: topic, key, value 사용.
                                                                                                                                // record 생성 방법 2: topic, value 사용.

        producer.send(record);      // send method: producer record -> broker에 전송할 message (event)
        producer.close();           // close.
    }

    public static void consumer(){
        Properties config = new Properties();   // Properties -> Consumer 설정 정보 저장.
        config.put(ConsumerConfig.GROUP_ID_CONFIG, "fo2s234dfsdf");                             // Consumer's group ID.
        config.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:10400");
        config.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringDeserializer");   // key deserializer
        config.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, "org.apache.kafka.common.serialization.StringDeserializer"); // value deserializer
        config.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG,"earliest");                 // 맨 처음 offset 사용! (항상 offset 0에서 시작)
        config.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "false");                  // auto commit 'off' 시킴 -> 그래서 항상 처음부터 다 찍이주는구나.

        KafkaConsumer<String, String> consumer = new KafkaConsumer<>(config);
        consumer.subscribe(Collections.singletonList("topic_name"));                            // Topic 구독!

        ConsumerRecords<String, String> records = consumer.poll(1000);              // Records를 batch로 받아와서,?!
        for (ConsumerRecord<String, String> record : records)                                   // for loop 돌면서 풀어줌?!
        {
            System.out.printf("topic = %s, partition = %s, offset = %d, customer = %s, country = %s\n",
                record.topic(), record.partition(), record.offset(), record.key(), record.value());
            // "topic = topic_name, partition = 0, offset = 8, customer = 12, country = message_example"
        }
        consumer.close();

    }

    // Java source program의 시작 지점?!
	public static void main(String[] args) {
		producer();
        consumer();
	}
}

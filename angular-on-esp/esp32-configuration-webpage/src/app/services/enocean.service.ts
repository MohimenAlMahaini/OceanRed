import { Inject, Injectable, isDevMode } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Subject } from 'rxjs';
import { EnoceanI } from '../enocean-i';

const devIP = "http://192.168.124.21:81";
// const devIP = "http://192.168.0.242:81";
const IP = isDevMode() ? devIP : '';
// const IP = '';

@Injectable({
  providedIn: 'root'
})
export class EnoceanService
{

  private requestInterval = 1500;
  private listening = false;

  private enoceanTelegramArray: EnoceanI[] = [];


  private receivableSubject = new Subject<string>();
  public receivableTelegram = this.receivableSubject.asObservable();

  constructor(private http: HttpClient) { }


  getEnoceanTelegramArray(): EnoceanI[]
  {
    let result: EnoceanI[] = [];
    this.enoceanTelegramArray.forEach((value) => result.push(value));
    return result;
  }

  resetEnoceanTelegramArray(): void
  {
    this.enoceanTelegramArray = [];
  }

  startListening(): void
  {
    if (!this.listening)
    {
      this.http.get(IP + '/start-enocean-teachin').subscribe((data) =>
      {
        console.log("START LISTENING TO ENOCEAN...");
        console.log(data);
      }, (error) => console.log(error), () => console.log("completed!"));; // Inform ESP32 to start.
      this.listening = true;
      this.startRequestLoop();
    }
  }

  private startRequestLoop(): void
  {
    if (this.listening)
    {
      setTimeout(() =>
      {

        if (!this.listening)
          return;

        console.log("Request EnOcean data...")
        this.http.get(IP + '/get-eo').subscribe((data) =>
        {
          console.log(data);
          if (data)
          {
            let container = data as any;
            if (container.telegrams)
            {
              for (let i = 0; i < container.telegrams.length; i++)
              {
                let telegram = container.telegrams[i];
                if (telegram)
                {
                  this.enoceanTelegramArray.push(telegram);
                  this.receivableSubject.next(telegram);
                }
              }
            }
          }

          // this.http.get<EnoceanI>('get-eo').subscribe((data: EnoceanI) =>  {
          //   console.log(data);
          //   if (data) {

          //   }

          //Notify observers

          this.startRequestLoop();
        },
          (error) =>
          {
            console.log("Error");
            console.log(error);

            this.startRequestLoop();
          });

      }, this.requestInterval);
    }
  }

  stopListening(): void
  {
    this.http.get(IP + "/stop-enocean-teachin").subscribe((data) =>
    {
      console.log("Stoped EnOcean Teachin");
      console.log(data);
    }, (error) => console.log(error), () => console.log("completed!")); // Inform ESP32 to stop.
    this.listening = false;
  }



}

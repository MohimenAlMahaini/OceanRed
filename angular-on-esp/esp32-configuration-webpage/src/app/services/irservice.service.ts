import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class IRServiceService {
  private requestInterval = 1500;
  private listening = false;

  constructor(private http: HttpClient) { }

  startListening(): void {
    if (!this.listening) {
      this.listening = true;
      this.startRequestLoop();
    }
  }

  stopListening(): void {
    this.listening = false;
  }

  private startRequestLoop(): void {
    if (this.listening) {
      setTimeout(() => {

        if (!this.listening)
          return;

        console.log("Request data...")
        this.http.get('get?request-ir-signal=1').subscribe((data) => {
          console.log("Got data:");
          console.log(data);

          //Store data
          //Notify observers

          this.startRequestLoop();
        },
          (error) => {
            console.log("Error");
            console.log(error);

            this.startRequestLoop();
          });

      }, this.requestInterval);
    }
  }
}
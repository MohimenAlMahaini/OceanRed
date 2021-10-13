import { HttpClient } from '@angular/common/http';
import { Component, OnInit } from '@angular/core';
import { IenoceanData } from '../ienocean-data';
import { IRServiceService } from '../services/irservice.service';

@Component({
  selector: 'app-enocean-table',
  templateUrl: './enocean-table.component.html',
  styleUrls: ['./enocean-table.component.css'],
})

export class EnoceanTableComponent implements OnInit {
  displayedColumns: string[] = ['PacketType', 'SenderID'];

  constructor(private http: HttpClient, private irService: IRServiceService) {
    irService.startListening();
  }

  ngOnInit(): void { }

  getPosts() {
    console.log("Sending Get Request TO ESP");
    this.http.get('get?start=1').subscribe((data) => {
      console.log("Request End");
      console.log(data);
    },
      (error) => {
        console.log("Error");
        console.log(error);
      },
      () => {
        console.log("completed")
      });
  }

  public addItem(pram: IenoceanData) { }

  public printItems() { }

  ngOnDestroy(): void {
    this.irService.stopListening();

  }

}

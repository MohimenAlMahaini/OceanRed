import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { NavigationBarComponent } from './navigation-bar/navigation-bar.component';
import { TeachinComponent } from './teachin/teachin.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { AboutusComponent } from './aboutus/aboutus.component';
import { OceanredComponent } from './oceanred/oceanred.component';
import { TeachInDialogComponent } from './teach-in-dialog/teach-in-dialog.component';

import { MatDialogModule } from '@angular/material/dialog'
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatButtonModule } from '@angular/material/button';
import { MatInputModule } from '@angular/material/input';
import { FormsModule } from '@angular/forms';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatCardModule } from '@angular/material/card';

import { MatTableModule } from '@angular/material/table'
import { MatRippleModule } from '@angular/material/core';

//
import { HttpClient, HttpClientModule } from '@angular/common/http';
import { TeachinEnOceanComponent } from './teachin-en-ocean/teachin-en-ocean.component';


@NgModule({
  declarations: [
    AppComponent,
    NavigationBarComponent,
    TeachinComponent,
    DashboardComponent,
    AboutusComponent,
    OceanredComponent,
    TeachInDialogComponent,
    TeachinEnOceanComponent,
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    MatDialogModule,
    BrowserAnimationsModule,
    MatButtonModule,
    MatInputModule,
    FormsModule,
    MatTableModule,
    HttpClientModule,     // For Testing
    MatExpansionModule,
    MatCardModule,
    MatRippleModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }

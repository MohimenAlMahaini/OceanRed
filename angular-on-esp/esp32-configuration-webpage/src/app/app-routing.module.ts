import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { AboutusComponent } from './aboutus/aboutus.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { OceanredComponent } from './oceanred/oceanred.component';
import { TeachinComponent } from './teachin/teachin.component';

const routes: Routes = [
  { path: '', component: DashboardComponent },
  { path: 'teachIn', component: TeachinComponent },
  { path: 'aboutus', component: AboutusComponent },
  { path: 'oceanred', component: OceanredComponent }
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }

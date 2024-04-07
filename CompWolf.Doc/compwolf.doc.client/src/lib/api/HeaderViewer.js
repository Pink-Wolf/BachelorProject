import { Declaration, Reference } from "./CodeComponents";
import BaseEntityViewer from "./BaseEntityViewer";
import { Fragment } from "react";

export default function HeaderViewer(props) {
	const data = props.data;

	const is_empty = (x) => { return x == undefined || x.length == 0 }

	return (
		<section>
			<small>
				<p id="Project">In project <Reference path={data.project}>{data.project}</Reference></p>
			</small>
			<big id="Declaration">
				<Declaration>{`<${data.header}>`}</Declaration>
			</big>
			<p id="Description">
				{data.detailedDescription}
			</p>
			<h1>Entities:</h1>
			<ul>
				{data.entities.map((x, i) => {
					return (
						<li key={i}>
							<Reference path={`${data.project}/${data.header}/${x.name}`}>{x.name}</Reference>
						</li>
					)
				})}
			</ul>
		</section>
	)
}